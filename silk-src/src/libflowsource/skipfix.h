/*
** Copyright (C) 2007-2017 by Carnegie Mellon University.
**
** @OPENSOURCE_LICENSE_START@
** See license information in ../../LICENSE.txt
** @OPENSOURCE_LICENSE_END@
*/

/*
**  skipfix.h
**
**    Converts between IPFIX and SiLK Flow Recs
**
*/
#ifndef _SKIPFIX_H
#define _SKIPFIX_H
#ifdef __cplusplus
extern "C" {
#endif

#include <silk/silk.h>

RCSIDENTVAR(rcsID_SKIPFIX_H, "$SiLK: skipfix.h efd886457770 2017-06-21 18:43:23Z mthomas $");

#include <silk/silk_types.h>
#include <silk/libflowsource.h>
#include <silk/skfixbuf.h>


/**
 * @file
 *
 * SiLK Flow record (rwRec) interface to fixbuf. Supports the reading
 * and writing of IPFIX Files by SiLK applications, and the creation
 * of IPFIX Collecting and Exporting Processes using the SiLK flow
 * format.
 *
 * This file is part of libflowsource.
 *
 * This library supports the creation of fbListener_t and fBuf_t
 * instances configured to read any IPFIX record containing the
 * appropriate information elements as a SiLK rwRec, and the creation
 * of fBuf_t instances configured to write SiLK rwRecs as IPFIX
 * records.
 *
 * To read SiLK Flow records from an IPFIX file, use fopen() to open
 * the file, create a buffer using skiCreateReadBufferForFP(), and
 * iterate over records with skiRwNextRecord().  When done, use
 * fBufFree() to free the resulting buffer and fclose() the file.
 *
 * To write SiLK Flow records to an IPFIX file, fopen() the file,
 * create a buffer using skiCreateWriteBufferForFP(), and write each
 * record with skiRwAppendRecord().  Use fBufFree() to free the
 * resulting buffer and fclose() the file.
 *
 * This library uses the GError facility from glib for reporting
 * errors.  Pass a pointer to a NULL GError * on any call taking an
 * err parameter; if an error occurs, the function will return NULL or
 * FALSE as appropriate, and allocate a GError structure describing
 * the error. If an error occurs, you can use the g_error_matches()
 * macro to match it against FB_ERROR_* constants defined by
 * libfixbuf, and the err->message field to get a human-readable error
 * message. After handling an error, use the g_clear_error() macro to
 * free the error description. See the glib documentation for more
 * details on this facility.
 *
 * See the documentation for libfixbuf for details on the fBuf_t,
 * fbListener_t, fbCollector_t, fbExporter_t, and fbConnSpec_t types,
 * and the fbListenerAppInit_fn and fbListenerAppFree_fn callbacks.
 */


/**
 * IPFIX Template ID for SiLK flows. Used as the external template ID
 * for SiLK Flow records written to IPFIX buffers.
 */
#define SKI_RWREC_TID        0xAFEA


/*
 *   Only allow files in the libflowsource directory to access the
 *   reading sections of this header.
 */
#ifdef LIBFLOWSOURCE_SOURCE

/* Types of IPFIX records */
typedef enum ski_rectype_en {
    SKI_RECTYPE_FLOW,
    SKI_RECTYPE_STATS,
    SKI_RECTYPE_NF9_SAMPLING,
    SKI_RECTYPE_UNKNOWN,
    SKI_RECTYPE_ERROR
} ski_rectype_t;


/* Bytes of padding to add to ski_yaf_stats to get a multiple of
 * 64bits */
#define SKI_YAF_STATS_PADDING  0

/* Record type for yaf statistics; these values are based on the yaf
 * 2.3.0 manual page.  This structure must be kept in sync with
 * ski_yaf_stats_option_spec[] defined in skipfix.c. */
typedef struct ski_yaf_stats_st {
    /* The time in milliseconds of the last (re-)initialization of
     * yaf.  IE 161, 8 octets */
    uint64_t    systemInitTimeMilliseconds;

    /* Total amount of exported flows from yaf start time.  IE 42, 8
     * octets */
    uint64_t    exportedFlowRecordTotalCount;

    /* Total amount of packets processed by yaf from yaf start time.
     * IE 86, 8 octets */
    uint64_t    packetTotalCount;

    /* Total amount of dropped packets according to statistics given
     * by libpcap, libdag, or libpcapexpress.  IE 135, 8 octets */
    uint64_t    droppedPacketTotalCount;

    /* Total amount of packets ignored by the yaf packet decoder, such
     * as unsupported packet types and incomplete headers, from yaf
     * start time.  IE 164, 8 octets */
    uint64_t    ignoredPacketTotalCount;

    /* Total amount of packets rejected by yaf because they were
     * received out of sequence.  IE 167, 8 octets */
    uint64_t    notSentPacketTotalCount;

    /* Total amount of fragments that have been expired since yaf
     * start time.  CERT (PEN 6871) IE 100, 4 octets */
    uint32_t    expiredFragmentCount;

#if 0
    /* Total number of packets that been assembled from a series of
     * fragments since yaf start time. CERT (PEN 6871) IE 101, 4
     * octets */
    uint32_t    assembledFragmentCount;

    /* Total number of times the yaf flow table has been flushed since
     * yaf start time.  CERT (PEN 6871) IE 104, 4 octets */
    uint32_t    flowTableFlushEventCount;

    /* The maximum number of flows in the yaf flow table at any one
     * time since yaf start time.  CERT (PEN 6871) IE 105, 4 octets */
    uint32_t    flowTablePeakCount;

    /* The mean flow rate of the yaf flow sensor since yaf start time,
     * rounded to the nearest integer.  CERT (PEN 6871) IE 102, 4
     * octets */
    uint32_t    meanFlowRate;

    /* The mean packet rate of the yaf flow sensor since yaf start
     * time, rounded to the nearest integer.  CERT (PEN 6871) IE 103,
     * 4 octets */
    uint32_t    meanPacketRate;

    /* The IPv4 Address of the yaf flow sensor.  IE 130, 4 octets */
    uint32_t    exporterIPv4Address;
#endif  /* 0 */

    /* The following is not currently used, but it is here for
     * alignment purposes. */
    /* Set the ID of the yaf flow sensor by giving a value to
     * --observation-domain.  The default is 0.   IE 144, 4 octets */
    uint32_t    exportingProcessId;

#if SKI_YAF_STATS_PADDING != 0
    uint8_t     pad[SKI_YAF_STATS_PADDING];
#endif
} ski_yaf_stats_t;

/**
 * Print to the named file handle information about the internal data
 * structures.  This can be used for debugging to ensure that the data
 * model does not contain holes or mis-aligned members.
 *
 * @param fh      the file handle on which to print the information
 */
void
skiCheckDataStructure(
    FILE               *fh);

/**
 * Create a IPFIX Collecting Process session listener for reading SiLK
 * Flow records. Used to wait for connections from IPFIX exporting
 * processes; use fbListenerWait() to accept connections and access
 * buffers suitable for use with skiRwNextRecord(). See the fixbuf
 * documentation for fbListenerAlloc() and fbListenerWait() for
 * details.
 *
 * @param spec    local endpoint connection specifier; specifies a port
 *                and transport protocol to use, and optionally the
 *                address of an interface to listen on.
 * @param appinit application connection initiation function. Called on each
 *                collection attempt; vetoes connection attempts and creates
 *                application context.
 * @param appfree application context free function.
 * @param err     an error description
 * @return a new listener, or NULL on failure.
 */
fbListener_t *
skiCreateListener(
    fbConnSpec_t           *spec,
    fbListenerAppInit_fn    appinit,
    fbListenerAppFree_fn    appfree,
    GError                **err);


/**
 * Create a buffer attached to a standard IO file pointer suitable for
 * use for skiRwNextRecord(). The file pointer must be opened for
 * reading. See the fixbuf documentation for fbCollectorAllocFile()
 * and/or fbCollectorAllocFP() for details on the fbCollector_t type.
 *
 * @param fp a file pointer
 * @param err an error description
 * @return a new buffer, or NULL on failure.
 */
fBuf_t *
skiCreateReadBufferForFP(
    FILE               *fp,
    GError            **err);


/**
 * Determine type of next record.
 *
 * @param fbuf      an IPFIX message buffer
 * @param err       an error description
 * @return The type of record in the buffer, as an ski_rectype_t
 */
ski_rectype_t
skiGetNextRecordType(
    fBuf_t             *fbuf,
    GError            **err);


/**
 * Read the next IPFIX record from a buffer and convert it to a SiLK
 * Flow record.  Overwrites the rwRec buffer pointed to by rec with
 * the converted record. If the next IPFIX record is a biflow record
 * and revRec is not NULL, overwrites the rwRec buffer pointed to by
 * revRec with the reverse direction record; if revRec is NULL, the
 * reverse direction is discarded silently.  If the next IPFIX record
 * is a uniflow record and revRec is not NULL, the the rwRec buffer
 * pointed to by revRec is cleared with RWREC_CLEAR().  Since every
 * flow record must contain at least one packet, the caller can check
 * that field on the rwRec buffer pointed to by revRec to determine
 * whether the IPFIX record was biflow or uniflow.
 *
 * Returns 1 if the IPFIX record contained a uni-flow record, or
 * returns 2 if the IPFIX record contained a bi-flow record.  Both rec
 * and revRec (if provided) will have been modified.
 *
 * Returns 0 if the IPFIX record should be ignored.  The forward rec
 * will have been cleared; the reverse record is untouched.  A record
 * can be ignored when (1)the record is IPv6 and SiLK is compiled
 * without IPv6 support, (2)the record has a packet and/or byte count
 * of 0, or (3)the record is explicitly marked as an "intermediate"
 * record by yaf.
 *
 * Returns -1 on failure.  The forward rec will have been cleared; the
 * reverse record is untouched.  On failure, check the error
 * description against FB_ERROR_EOF in domain FB_ERROR_DOMAIN for end
 * of file or connection normally closed by the remote end;
 * FB_ERROR_EOM for end of message (if the buffer was returned by
 * fbListenerWait() or was made manual by fBufSetAutomaticMode()); or
 * FB_ERROR_NLREAD to check for "no packet" (which is possible on
 * timeout over certain transport layers, or if a blocking read call
 * is interrupted).
 *
 * @param fbuf             an IPFIX Message buffer.
 * @param probe            the probe where the message was received
 * @param rec              pointer to forward direction SiLK Flow record.
 * @param revRec           pointer to reverse direction SiLK Flow record,
 *                         or NULL to ignore.
 * @param err              an error description
 * @return 1 for a uni-flow record, 2 for a bi-flow record, -1 on error,
 *                         0 on ignored record,
 */
int
skiRwNextRecord(
    fBuf_t                 *fbuf,
    const skpc_probe_t     *probe,
    skIPFIXSourceRecord_t  *forward_rec,
    skIPFIXSourceRecord_t  *reverse_rec,
    GError                **err);


/**
 * Read the next IPFIX record from a buffer and convert it to a yaf
 * stats record.
 *
 * @param fbuf             an IPFIX Message buffer.
 * @param probe            the probe where the message was received
 * @param stats            yaf stats record
 * @param err              an error description
 * @return TRUE on success, FALSE on error
 */
gboolean
skiYafNextStats(
    fBuf_t                 *fbuf,
    const skpc_probe_t     *probe,
    ski_yaf_stats_t        *stats,
    GError                **err);


/**
 * Read a NetFlowV9 Options Record regarding the flow sampling rate
 * and write a message to the log file.
 *
 * @param fbuf             an IPFIX Message buffer.
 * @param probe            the probe where the message was received
 * @param err              an error description
 * @return TRUE on success, FALSE on error
 */
gboolean
skiNextSamplingOptionsTemplate(
    fBuf_t                 *fbuf,
    const skpc_probe_t     *probe,
    GError                **err);


/**
 *     Add a callback to be invoked whenever the session receives a new
 *     template.  The purpose of the callback is the tell fixbuf how
 *     to process items in a subTemplateMultiList.
 *
 * @param session           the session on which to set the callback
 */
void
skiAddSessionCallback(
    fbSession_t        *session);

#endif  /* LIBFLOWSOURCE_SOURCE */


/**
 * Perform any necessary initialization.  Exit the program on error.
 */
void
skiInitialize(
    void);


/**
 * Free the memory associated with the Info Model---note that doing so
 * is not tread safe.
 */
void
skiTeardown(
    void);


#ifdef __cplusplus
}
#endif
#endif /* _SKIPFIX_H */

/*
** Local Variables:
** mode:c
** indent-tabs-mode:nil
** c-basic-offset:4
** End:
*/
