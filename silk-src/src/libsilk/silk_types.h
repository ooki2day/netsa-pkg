/*
** Copyright (C) 2011-2017 by Carnegie Mellon University.
**
** @OPENSOURCE_LICENSE_START@
** See license information in ../../LICENSE.txt
** @OPENSOURCE_LICENSE_END@
*/

/*
**  silk_types.h
**
**    A place to gather commonly used defines, typedefs, and enumerations.
**
*/

#ifndef _SILK_TYPES_H
#define _SILK_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <silk/silk.h>

RCSIDENTVAR(rcsID_SILK_TYPES_H, "$SiLK: silk_types.h efd886457770 2017-06-21 18:43:23Z mthomas $");

/**
 *  @file
 *
 *    The typedefs and some common macros used throughout the SiLK
 *    code base.
 *
 *    This file is part of libsilk.
 */


/* *****  IP ADDRESS / SOCKADDR  ********************************** */

/**
 *    The IP address structure.  Most code should use skipaddr_t
 *    instead of skIPUnion_t.  Users should not deference directly,
 *    use the macros specified in skipaddr.h to get/set the value.
 */
typedef union skipunion_un {
    uint32_t    ipu_ipv4;
    uint8_t     ipu_ipv6[16];
} skIPUnion_t;
typedef skIPUnion_t ipUnion                 SK_GCC_DEPRECATED;

/**
 *    An IP address structure that knows the version of IP address it
 *    contains.  Do not deference directly, instead use the macros
 *    specified in skipaddr.h to get/set the value
 */
typedef struct skipaddr_st {
    skIPUnion_t ip_ip;
    unsigned    ip_is_v6 :1;
} skipaddr_t;

/**
 *    Flags that determine the string representation of an IP as
 *    returned by skipaddrString() and other functions declared in
 *    utils.h
 */
typedef enum {
    /**
     *  Canonical form: dotted quad for IPv4 or hexadectet for
     *  IPv6. This uses inet_ntop(3) which prints ::ffff:0:0/96 and
     *  some parts of ::/96 using a mixture of IPv6 and IPv4.
     *
     *  The maximum length of this string is 15 for IPv4, 39 for IPv6,
     *  18 for IPv4/CIDR, 43 for IPv6/CIDR.
     */
    SKIPADDR_CANONICAL,
    /**
     *  Similar to the canonical form, except pad each octet of an
     *  IPv4 with leading zeros so the octet has exactly three
     *  characters.  For an IPv6 address, do not use :: shortening and
     *  pad each hexadectet with leading zeros so it has exactly four
     *  characters.  Useful for string comparisons.
     *
     *  String lengths are the same as canonical.
     */
    SKIPADDR_ZEROPAD,
    /**
     *  Value as an integer number, printed as decimal.
     *
     *  The maximum length of this string is 10 for IPv4, 39 for IPv6,
     *  13 for IPv4/CIDR, 43 for IPv6/CIDR.
     */
    SKIPADDR_DECIMAL,
    /**
     *  Value as an integer number, printed as hexadecimal.
     *
     *  The maximum length of this string is 8 for IPv4, 32 for IPv6,
     *  11 for IPv4/CIDR, 36 for IPv6/CIDR.
     */
    SKIPADDR_HEXADECIMAL,
    /**
     *  Similar to the canonical format of IPv6, but never uses a
     *  mixture of IPv4 and IPv6, and IPv4 addresses are always printed
     *  in the ::ffff:0:0/96 netblock.
     *
     *  String lengths are the same as canonical unless only IPv4
     *  addresses are present.  For that case, maximum string length
     *  is 16 for IPv4-as-IPv6, 20 for IPv4-as-IPv6/CIDR.
     */
    SKIPADDR_FORCE_IPV6,
    /**
     *  Identical to the canonical form unless the address is an IPv6
     *  address in the ::ffff:0:0/96 netblock, in which case the
     *  address is converted to IPv4 and displayed in the canonical
     *  form.
     *
     *  String lengths are the same as canonical.
     */
    SKIPADDR_AS_IPV4
} skipaddr_flags_t;

/**
 *    How to handle IPv6 Flow records.
 */
typedef enum sk_ipv6policy_en {
    /** completely ignore IPv6 flows */
    SK_IPV6POLICY_IGNORE = -2,
    /** convert IPv6 flows to IPv4 if possible, else ignore */
    SK_IPV6POLICY_ASV4 = -1,
    /** mix IPv4 and IPv6 flows in the result--this is the default */
    SK_IPV6POLICY_MIX = 0,
    /** force IPv4 flows to be converted to IPv6 */
    SK_IPV6POLICY_FORCE = 1,
    /** only return IPv6 flows that were marked as IPv6 */
    SK_IPV6POLICY_ONLY = 2
} sk_ipv6policy_t;

/**
 *    Length of buffer required to hold an IPv6 address.  This is
 *    taken from INET6_ADDRSTRLEN used by inet_ntop(), which can
 *    return "0000:0000:0000:0000:0000:00FF:000.000.000.000"
 */
#define SK_NUM2DOT_STRLEN 46

/**
 *    A special structure of IP Addresses.  It is defined in utils.h
 */
typedef struct skIPWildcard_st skIPWildcard_t;


/**
 *    A union that encompasses the various struct sockaddr types.
 *    Macros and functions for manipulating these are in utils.h.
 */
typedef union sk_sockaddr_un {
    struct sockaddr     sa;
    struct sockaddr_in  v4;
    struct sockaddr_in6 v6;
    struct sockaddr_un  un;
} sk_sockaddr_t;

/**
 *    The sk_sockaddr_array_t structure represents multiple
 *    representations of an address and/or port.  Macros and functions
 *    for manipulating these are in utils.h.
 */
typedef struct sk_sockaddr_array_st {
    /* the host-name/-address or NULL for INADDR_ANY */
    char           *name;
    /* the host:port pair; uses '*' for INADDR_ANY */
    char           *host_port_pair;
    /* array of sockets */
    sk_sockaddr_t  *addrs;
    /* number of entries in 'addrs' */
    uint32_t        num_addrs;
} sk_sockaddr_array_t;


/* *****  TIME  *************************************************** */

/**
 *    sktime_t is milliseconds since the UNIX epoch.  Macros and
 *    functions for manipulating these are in utils.h.
 *
 *    Value is signed, like time_t.
 */
typedef int64_t sktime_t;

/**
 *    Minimum size of buffer to pass to sktimestamp_r().
 */
#define SKTIMESTAMP_STRLEN 28


/* *****  FLOW RECORDS (RWREC)  *********************************** */

/**
 *    The generic SiLK Flow record returned from ANY file format
 *    containing packed SiLK Flow records.  It is defined in rwrec.h
 */
typedef struct rwGenericRec_V6 rwRec;


/**
 *    The maximum size of a SiLK Flow record.
 */
#define SK_MAX_RECORD_SIZE      144

/**
 *    Number of possible SNMP interface index values
 */
#define SK_SNMP_INDEX_LIMIT   65536

/* *****  SCHEMAS, RECORDS, AND FIELDS **************************** */

/**
 *    A schema represents a set of sk_field_t's and maintains the
 *    fields' offsets.  Schemas are reference counted, with
 *    sk_schema_destroy() decrementing the count, and
 *    sk_schema_clone() incrementing the count.  A schema can have
 *    multiple copies of the same field.
 */
struct sk_schema_st;
typedef struct sk_schema_st sk_schema_t;

/**
 *    A record consists of data, and an sk_schema_t describing the
 *    data.
 *
 *    Schemas are referenced by records.  When a record is destroyed,
 *    the schema's refcount is decremented.
 */
struct sk_fixrec_st;
typedef struct sk_fixrec_st sk_fixrec_t;

/**
 *    A field consists of name and type information represented by an
 *    fbInfoElement_t, and a set of basic operations represented by an
 *    sk_field_ops_t.  A field also contains an offset which is
 *    maintained by any owning schema.
 */
struct sk_field_st;
typedef struct sk_field_st sk_field_t;

/**
 *    A sidecar data structure describes the type of fields that may
 *    appear on the sidecar element of an rwRec.
 */
struct sk_sidecar_st;
typedef struct sk_sidecar_st sk_sidecar_t;


/* *****  STREAM / FILE FORMATS  ********************************** */

/**
 *    Interface to a file containing SiLK Data---flow records or
 *    IPsets, etc---is an skstream_t.  See skstream.h.
 */
typedef struct skstream_st skstream_t;

/**
 *    Type to hold the ID of the various SiLK file formats.  The
 *    format IDs begin with FT_ and are listed in silk_files.h.
 */
typedef uint8_t  sk_file_format_t;
typedef sk_file_format_t fileFormat_t       SK_GCC_DEPRECATED;

/**
 *    The value for an invalid or unrecognized file format.
 *
 *    Since SiLK 3.13.0.
 */
#define SK_INVALID_FILE_FORMAT      ((sk_file_format_t)0xFF)

/**
 *    The strlen() of the names of file formats will be this size or
 *    less.
 */
#define SK_MAX_STRLEN_FILE_FORMAT   32

/**
 *    A version of the file format.
 */
typedef uint8_t  sk_file_version_t;
typedef sk_file_version_t fileVersion_t     SK_GCC_DEPRECATED;

/**
 *    Value meaning that any file version is valid
 */
#define SK_RECORD_VERSION_ANY       ((sk_file_version_t)0xFF)

/**
 *    The compression method used to write the data section of a file.
 *    The known compression methods are listed in silk_files.h.
 */
typedef uint8_t sk_compmethod_t;

/**
 *    The value for an invalid or unrecognized compression method
 */
#define SK_INVALID_COMPMETHOD       ((sk_compmethod_t)0xFF)

/**
 *    Values that specify how a stream/file is to be opened.
 */
typedef enum {
    SK_IO_READ = 1,
    SK_IO_WRITE = 2,
    SK_IO_APPEND = 4
} skstream_mode_t;

/**
 *    What type of content the stream contains
 */
typedef enum {
    /** stream contains line-oriented text */
    SK_CONTENT_TEXT = (1 << 0),
    /** stream contains a SiLK file header and SiLK Flow data */
    SK_CONTENT_SILK_FLOW = (1 << 1),
    /** stream contains a SiLK file header and data (non-Flow data) */
    SK_CONTENT_SILK = (1 << 2),
    /** stream contains binary data other than SiLK or IPFIX data */
    SK_CONTENT_OTHERBINARY = (1 << 3)
} skcontent_t;


/**
 *    A wrapper over skstream_t that supports reading and writing
 *    streams of IPFIX records.
 */
typedef struct sk_fixstream_st sk_fixstream_t;


/* *****  CLASS / TYPE / SENSORS  ********************************* */

/* Most of the functions for manipulating these are declared in
 * sksite.h */

/**
 *    Type to hold a class ID.  A class is not actually stored in
 *    packed records (see sk_flowtype_id_t).
 */
typedef uint8_t sk_class_id_t;
typedef sk_class_id_t classID_t             SK_GCC_DEPRECATED;

/**
 *    The maximum number of classes that may be allocated.  (All valid
 *    class IDs must be less than this number.)
 */
#define SK_MAX_NUM_CLASSES          ((sk_class_id_t)32)

/**
 *    The value for an invalid or unrecognized class.
 */
#define SK_INVALID_CLASS            ((sk_class_id_t)0xFF)

/**
 *    A flowtype is a class/type pair.  It has a unique name and
 *    unique ID.
 */
typedef uint8_t  sk_flowtype_id_t;
typedef sk_flowtype_id_t flowtypeID_t       SK_GCC_DEPRECATED;

/**
 *    The maximum number of flowtypes that may be allocated.  (All
 *    valid flowtype IDs must be less than this number.)
 */
#define SK_MAX_NUM_FLOWTYPES        ((sk_flowtype_id_t)0xFF)

/**
 *    The value for an invalid or unrecognized flow-type value
 */
#define SK_INVALID_FLOWTYPE         ((sk_flowtype_id_t)0xFF)

/**
 *    The strlen() of the names of flowtypes, classes, and types will
 *    be this size or less.  Add 1 to allow for the NUL byte.
 */
#define SK_MAX_STRLEN_FLOWTYPE      32

/**
 *    Type to hold a sensor ID.  Usually, a sensor is a router or
 *    other flow collector.
 */
typedef uint16_t sk_sensor_id_t;
typedef sk_sensor_id_t sensorID_t           SK_GCC_DEPRECATED;

/**
 *    The maximum number of sensors that may be allocated.  (All valid
 *    sensor IDs must be less than this number.
 */
#define SK_MAX_NUM_SENSORS          ((sk_sensor_id_t)0xFFFF)

/**
 *    The value for an invalid or unrecognized sensor.
 */
#define SK_INVALID_SENSOR           ((sk_sensor_id_t)0xFFFF)

/**
 *    The maximum length of a sensor name, not including the final
 *    NUL.
 */
#define SK_MAX_STRLEN_SENSOR        64

/**
 *    Type to hold a sensor group ID.  This is not actually stored in
 *    packed records.
 */
typedef uint8_t sk_sensorgroup_id_t;
typedef sk_sensorgroup_id_t sensorgroupID_t SK_GCC_DEPRECATED;

/**
 *    The maximum number of sensorgroups that may be allocated.  (All
 *    valid sensorgroup IDs must be less than this number.)
 */
#define SK_MAX_NUM_SENSORGROUPS     ((sk_sensorgroup_id_t)0xFF)

/**
 *    The value for an invalid or unrecognized sensor.
 */
#define SK_INVALID_SENSORGROUP      ((sk_sensorgroup_id_t)0xFF)

/**
 *    The sksite_repo_key_t is used to specify the location of a file
 *    in a SiLK repository.
 */
struct sksite_repo_key_st {
    /* the hour that this file is for */
    sktime_t            timestamp;
    /* the sensor that this file is for */
    sk_sensor_id_t      sensor_id;
    /* the flowtype (class/type) that this file is for */
    sk_flowtype_id_t    flowtype_id;
};
typedef struct sksite_repo_key_st sksite_repo_key_t;


/* *****  BITMPAP / LINKED-LIST / VECTOR  ************************* */

/**
 *    Bitmap of integers.  It is defined in utils.h.
 */
typedef struct sk_bitmap_st sk_bitmap_t;


/**
 *    Signature of a doubly-linked list.  See skdllist.h.
 */
struct sk_dllist_st;
typedef struct sk_dllist_st      sk_dllist_t;

/**
 *    Signature of an iterator for a doubly linked list
 */
struct sk_dll_iter_st;
typedef struct sk_dll_iter_st sk_dll_iter_t;
struct sk_dll_iter_st {
    void           *data;
    sk_dll_iter_t  *link[2];
};

/**
 *    A stringmap maps strings to integer IDs.  It is used for parsing
 *    the user's argument to --fields.  See skstringmap.h.
 */
typedef sk_dllist_t sk_stringmap_t;

/**
 *    Growable array.  See skvector.h.
 */
typedef struct sk_vector_st sk_vector_t;



/* *****  IPSET  ************************************************** */

/**
 *    Data structure to hold a set of IP addresses.  See skipset.h.
 */
typedef struct skipset_st skipset_t;



/* *****  MISCELLANEOUS  ****************************************** */

/**
 *    An enumeration type for endianess.
 */
typedef enum silk_endian_en {
    SILK_ENDIAN_BIG,
    SILK_ENDIAN_LITTLE,
    SILK_ENDIAN_NATIVE,
    SILK_ENDIAN_ANY
} silk_endian_t;


/**
 *    The status of an iterator.
 */
typedef enum skIteratorStatus_en {
    /** More entries */
    SK_ITERATOR_OK=0,
    /** No more entries */
    SK_ITERATOR_NO_MORE_ENTRIES
} skIteratorStatus_t;

/**
 *    The type of message functions.  These should use the same
 *    semantics as printf.
 */
typedef int (*sk_msg_fn_t)(const char *, ...)
    SK_CHECK_TYPEDEF_PRINTF(1, 2);

/**
 *    The type of message functions with the arguments expanded to a
 *    variable argument list.
 */
typedef int (*sk_msg_vargs_fn_t)(const char *, va_list)
    SK_CHECK_TYPEDEF_PRINTF(1, 0);

#ifdef __cplusplus
}
#endif
#endif /* _SILK_TYPES_H */

/*
** Local Variables:
** mode:c
** indent-tabs-mode:nil
** c-basic-offset:4
** End:
*/
