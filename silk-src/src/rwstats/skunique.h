/*
** Copyright (C) 2001-2017 by Carnegie Mellon University.
**
** @OPENSOURCE_LICENSE_START@
** See license information in ../../LICENSE.txt
** @OPENSOURCE_LICENSE_END@
*/

/*
**  skunique.h
**
**    A library used by rwuniq and rwstats to bin records by a key and
**    compute aggregate values (e.g., sum of bytes) for each bin.
**
*/
#ifndef _SKUNIQUE_H
#define _SKUNIQUE_H 1
#ifdef __cplusplus
extern "C" {
#endif

#include <silk/silk.h>

RCSIDENTVAR(rcsID_SKUNIQUE_H, "$SiLK: skunique.h efd886457770 2017-06-21 18:43:23Z mthomas $");

#include <silk/silk_types.h>
#include <silk/skflowiter.h>


/* LOCAL DEFINES AND TYPEDEFS */

/*
 *    The sk_fieldid_t enumeration lists all the fields that can be added
 *    to an sk_fieldlist_t.  This lists includes key-fields (the
 *    fields on rwRecs) and value fields (such as sum of bytes).  The
 *    SK_FIELD_CALLER allows the caller to create arbitrary fields.
 *
 *    The definition of the enumeration is below.
 */
/* typedef enum sk_fieldid_en sk_fieldid_t; */

/*
 *    The sk_fieldlist_t holds a list of fields.  Use the
 *    skFieldList*() functions to create and modify the
 *    sk_fieldlist_t.  With the exception of SK_FIELD_CALLER, a field
 *    is added to sk_fieldlist_t by specifying a sk_fieldid_t.  To add
 *    an SK_FIELD_CALLER, the caller must fill-in the
 *    sk_fieldlist_entrydata_t structure with the required lengths and
 *    function pointers.
 */
typedef struct sk_fieldlist_st sk_fieldlist_t;

/*
 *    When a field is added to field list, the caller gets a
 *    sk_fieldentry_t has a handle to that field in that particular
 *    field_list.
 */
typedef struct sk_fieldentry_st sk_fieldentry_t;

/*
 *    The sk_fieldlist_iterator_t supports iterating over entries in
 *    an sk_fieldlist_t.
 */
typedef struct sk_fieldlist_iterator_st sk_fieldlist_iterator_t;


/*
 *    The following specify the signature of the callback functions
 *    the must be specified in the sk_fieldlist_entrydata_t structure
 *    when adding an SK_FIELD_CALLER field to an sk_fieldlist_t.
 */

/*
 *    Callback that serves two purposes:
 *
 *    When used as the 'rec_to_bin' member, it takes the record 'rec'
 *    and stores on 'dest' the binary value used by the other callback
 *    functions that operate on a binary file.
 *
 *    When used as the 'add_rec_to_bin' member, it takes the record
 *    'rec', computes the binary value for that record, and then adds
 *    that value to the aggregate binary value already present in
 *    'dest'.
 *
 *    -- if we want this to work with the existing plug-in API, do we
 *       need to include the "extra" value here?
 */
typedef void (*sk_fieldlist_rec_to_bin_fn_t)(
    const rwRec        *rec,
    uint8_t            *dest,
    void               *ctx);

/*
 *    Callback used as the 'bin_compare' member for comparing two
 *    aggregate binary values.
 */
typedef int (*sk_fieldlist_bin_cmp_fn_t)(
    const uint8_t      *bin1,
    const uint8_t      *bin2,
    void               *ctx);

/*    Callback for initializing a binary value.  Currently unused. */
typedef void (*sk_fieldlist_bin_init_fn_t)(
    uint8_t            *bin,
    void               *ctx);

/*    Callback used as the 'bin_merge' member for merging (for
 *    example, adding) two aggregate binary values--used for aggregate
 *    values (and maybe distinct fields?) */
typedef void (*sk_fieldlist_bin_merge_fn_t)(
    uint8_t            *bin1,
    const uint8_t      *bin2,
    void               *ctx);

/*    Currently unused.  I think this is here for consistency with
 *    skplugin.h
 */
typedef void (*sk_fieldlist_output_fn_t)(
    const uint8_t      *bin,
    void               *ctx);

/*
 *    Callback used as the 'bin_get_data' member.  This is used when a
 *    key field contains a pointer to the actual data to be used when
 *    hashing a fieldlist.  May be NULL when the key field is not a
 *    pointer.
 */
typedef uint8_t (*sk_fieldlist_bin_get_data_fn_t)(
    const uint8_t      *bin,
    uint8_t             out_data[],
    uint8_t             bin_len,
    void               *ctx);


/*
 *    The sk_fieldlist_entrydata_t structure is used when the caller
 *    wants to create a custom field.  This structure holds pointers
 *    to the above callback functions.
 *
 *    In addition, 'bin_octets' specifies the length of the custom
 *    field, and 'initial_value' specifies the value that the field
 *    should be initialized to.  If 'initial_value' is not provided,
 *    the field is memset() to 0.
 */
struct sk_fieldlist_entrydata_st {
    sk_fieldlist_rec_to_bin_fn_t    rec_to_bin;
    sk_fieldlist_bin_cmp_fn_t       bin_compare;
    sk_fieldlist_rec_to_bin_fn_t    add_rec_to_bin;
    sk_fieldlist_bin_merge_fn_t     bin_merge;
    sk_fieldlist_output_fn_t        bin_output;
    sk_fieldlist_bin_get_data_fn_t  bin_get_data;
    const uint8_t                  *initial_value;
    size_t                          bin_octets;
};
typedef struct sk_fieldlist_entrydata_st sk_fieldlist_entrydata_t;


/*
 *  ok = skFieldListCreate(&field_list);
 *
 *    Create a new field list at the location specified by
 *    'field_list'.  Return 0 on success, or -1 on failure.
 */
int
skFieldListCreate(
    sk_fieldlist_t    **field_list);

/*
 *  skFieldListDestroy(&field_list);
 *
 *    Destroy the field list at location 'field_list' and set the
 *    pointer to NULL.  Function is a no-op if the argument is NULL or
 *    if it references a NULL address.
 */
void
skFieldListDestroy(
    sk_fieldlist_t    **field_list);

/*
 *  entry = skFieldListAddField(field_list, regdata, ctx);
 *
 *    Add a custom field to 'field_list', where the structure
 *    'regdata' specifies the behavior of the field.  'ctx' is a
 *    user-defined context that will be passed to the comparison
 *    functions.  Returns a handle to the new field, or NULL if there
 *    is an error.
 */
sk_fieldentry_t *
skFieldListAddField(
    sk_fieldlist_t                 *field_list,
    const sk_fieldlist_entrydata_t *regdata,
    void                           *ctx);

/*
 *  entry = skFieldListAddKnownField(field_list, field_id, ctx);
 *
 *    Add a known sk_fieldid_t to 'field_list'.  'ctx' is a
 *    user-defined context that will be passed to the comparison
 *    functions.
 */
sk_fieldentry_t *
skFieldListAddKnownField(
    sk_fieldlist_t     *field_list,
    int                 field_id,
    void               *ctx);

/*
 *  size = skFieldListGetBufferSize(field_list);
 *
 *    Return the number of octets required to hold all the fields in
 *    'field_list.
 */
size_t
skFieldListGetBufferSize(
    const sk_fieldlist_t   *field_list);

/*
 *    The following functions work with the handle that was returned
 *    by the call to skFieldListAddKnownField() and
 *    skFieldListAddField().
 */

/*
 *  ctx = skFieldListEntryGetContext(field);
 *
 *    Return the context object that was specified when 'field' was
 *    created.
 */
void *
skFieldListEntryGetContext(
    const sk_fieldentry_t  *field);

/*
 *  field_id = skFieldListEntryGetId(field);
 *
 *    Return the sk_fieldid_t value for 'field'.
 */
uint32_t
skFieldListEntryGetId(
    const sk_fieldentry_t  *field);

/*
 *  size = skFieldListEntryGetBinOctets(field);
 *
 *    Return the number of octets required to hold 'field'.
 */
size_t
skFieldListEntryGetBinOctets(
    const sk_fieldentry_t  *field);

/*
 *  skFieldListExtractFromBuffer(field_list,all_fields_buf,field,one_field_buf);
 *
 *    Copy the value associated with 'field' from the 'all_fields_buf'
 *    into the 'one_field_buf'.  It is the caller's responsibility to
 *    ensure that 'one_field_buf' is large enough to hold the value.
 */
void
skFieldListExtractFromBuffer(
    const sk_fieldlist_t   *field_list,
    const uint8_t          *all_fields_buffer,
    sk_fieldentry_t        *field_entry,
    uint8_t                *one_field_buf);



/*
 *    The following support iteration over the fields in a field_list.
 */

/* Do we still need the structure of the field iterator as public? */
/* typedef struct sk_fieldlist_iterator_st sk_fieldlist_iterator_t; */
struct sk_fieldlist_iterator_st {
    const sk_fieldlist_t   *field_list;
    size_t                  field_idx;
};


/*
 *  skFieldListIteratorBind(field_list, *iter);
 *
 *    Bind the field list iterator 'iter' to 'field_list'.
 */
void
skFieldListIteratorBind(
    const sk_fieldlist_t       *field_list,
    sk_fieldlist_iterator_t    *iter);

/*
 *  field = skFieldListIteratorNext(iter);
 *
 *    Return a handle to the next field in the field_list.  Return
 *    NULL once the iterator 'iter' has visited every field in the
 *    field list.
 */
sk_fieldentry_t *
skFieldListIteratorNext(
    sk_fieldlist_iterator_t    *iter);


/*
 *    As noted above, the sk_fieldid_t enumeration lists all the
 *    fields that can be added to an sk_fieldlist_t.
 */
typedef enum sk_fieldid_en {
    /* the following correspond to values in rwrec.h */
    SK_FIELD_SIPv4,             /*  0 */
    SK_FIELD_DIPv4,
    SK_FIELD_SPORT,
    SK_FIELD_DPORT,

    SK_FIELD_PROTO,             /*  4 */
    SK_FIELD_PACKETS,
    SK_FIELD_BYTES,
    SK_FIELD_FLAGS,

    SK_FIELD_STARTTIME,         /*  8 */
    SK_FIELD_ELAPSED,
    SK_FIELD_ENDTIME,
    SK_FIELD_SID,

    SK_FIELD_INPUT,             /* 12 */
    SK_FIELD_OUTPUT,
    SK_FIELD_NHIPv4,
    SK_FIELD_INIT_FLAGS,

    SK_FIELD_REST_FLAGS,        /* 16 */
    SK_FIELD_TCP_STATE,
    SK_FIELD_APPLICATION,
    SK_FIELD_FTYPE_CLASS,

    SK_FIELD_FTYPE_TYPE,        /* 20 */
    SK_FIELD_ICMP_TYPE,
    SK_FIELD_ICMP_CODE,
    /* the above correspond to values in rwrec.h */

    SK_FIELD_SIPv6,
    SK_FIELD_DIPv6,             /* 24 */

    SK_FIELD_NHIPv6,
    SK_FIELD_RECORDS,
    SK_FIELD_SUM_PACKETS,
    SK_FIELD_SUM_BYTES,         /* 28 */

    SK_FIELD_SUM_ELAPSED,
    SK_FIELD_MIN_STARTTIME,
    SK_FIELD_MAX_ENDTIME,
    SK_FIELD_CALLER             /* 32 */
} sk_fieldid_t;



/* ************************************************************ */
/* ************************************************************ */

/*
 *    The sk_unique_t structure supports binning records, computing
 *    some value for all the records in a bin, and looping over the
 *    bins.
 *
 *    sk_unique_t was designed with batch processing in mind, such as
 *    that done by rwuniq and rwstats.  As currently written, you
 *    cannot use skUniqueIteratorCreate() until after you call
 *    skUniquePrepareForOutput(), and you cannot skUniqueAddRecord()
 *    once you call skUniquePrepareForOutput().  Things that occur in
 *    skUniquePrepareForOutput() are:
 *
 *        Dumping last batch of records to disk if we have already
 *        created temporary files.  To address this, we could have the
 *        sk_unique_t object return an error if the hash tables are
 *        too large to fit into memory.
 *
 *        Sorting the keys of the table when sorted output is
 *        requested.  To avoid this issue, do not request sorted
 *        output.
 *
 *        Sorting the keys of the table when any distinct field is
 *        present.  This may be a little more difficult to address in
 *        environments such as the Analysis Pipeline.
 *
 *    sk_unique_t handles data in random or in presorted order.  The
 *    sk_sort_unique_t structure defined later in this file operates
 *    specifically on presorted input.
 *
 *    The general processing loop for sk_unique_t is:
 *
 *      skUniqueCreate(&u);
 *      skUniqueSetFields(u, key, distinct, value);
 *      skUniqueSetTempDirectory(u);  //optional
 *      skUniqueSetSortedOutput(u);   //optional
 *      skUniquePrepareForInput(u);
 *      foreach (input_file) {
 *          skStreamOpenSilkFlow(stream, input_file[i]);
 *          while (skStreamReadRecord(stream, rec)) {
 *            skUniqueAddRecord(rec);
 *          }
 *      }
 *      skUniquePrepareForOutput(u);
 *      skUniqueIteratorCreate(u, &iter);
 *      while (skUniqueIteratorNext(iter, &key_buf, &dist_buf, &val_buf)) {
 *        // print result
 *      }
 *      skUniqueIteratorDestroy(&iter);
 *      skUniqueDestroy(&u);
 */
typedef struct sk_unique_st sk_unique_t;

/*
 *    The sk_unique_iterator_t structure supports iterating over the
 *    bins once records have been added.  The internal structure of
 *    the iterator will differ depending on various settings of the
 *    sk_unique_t structure the iterator is bound to.  These
 *    differences should not concern the caller.
 */
typedef struct sk_unique_iterator_st sk_unique_iterator_t;

/*
 *  ok = skUniqueCreate(&uniq);
 *
 *    Create a new unique object and store its address in the
 *    parameter 'uniq'.  Return -1 if the unique object cannot be
 *    created.
 */
int
skUniqueCreate(
    sk_unique_t       **uniq);

/*
 *  skUniqueDestroy(&uniq);
 *
 *    Destroy the unique object whose address is specified in 'uniq'.
 *    Remove any temporary files.  This function may be called with a
 *    NULL pointer.
 */
void
skUniqueDestroy(
    sk_unique_t       **uniq);

/*
 *  ok = skUniqueSetSortedOutput(uniq);
 *
 *    Specify that the unique object 'uniq' should present its results
 *    in sorted order.  This function must be called before calling
 *    skUniquePrepareForInput(); otherwise, the function returns -1.
 */
int
skUniqueSetSortedOutput(
    sk_unique_t        *uniq);

/*
 *  skUniqueSetTempDirectory(uniq, temp_dir);
 *
 *    Specify that the unique object 'uniq' should use the directory
 *    'temp_dir' to store temporary files.  This function is a no-op
 *    if it is called after skUniquePrepareForInput() has been called.
 *    This function makes a copy of the parameter 'temp_dir'.
 */
void
skUniqueSetTempDirectory(
    sk_unique_t        *uniq,
    const char         *temp_dir);

/*
 *  ok = skUniqueSetFields(uniq, keys, distincts, values);
 *
 *    Specify the fields that the unique object 'uniq' should use.  It
 *    is an error to call this function after
 *    skUniquePrepareForInput() has been called.
 *
 *    'keys' is a field list containing the fields to bin by.  It must
 *    contain at least one field.
 *
 *    'distincts' is a field list containing the fields where the
 *    caller wishes to get a count of distinct values per key.  It may
 *    be NULL or contain no fields, subject to the additional
 *    constraints given below.  The keys specified in 'distincts' may
 *    not be present in 'keys'.
 *
 *    'values' is a field list containing the fields where the caller
 *    wishes to compute an aggregate value per key.  It may be NULL or
 *    contain no fields, subject to the additional containsts given
 *    below.
 *
 *    If this function is called multiple times, the previous field
 *    values will be lost.
 *
 *    There are additional constaints, which will be checked when
 *    skUniquePrepareForInput() is called:
 *
 *    The length of 'keys' and 'distincts' must total to no more than
 *    HASHLIB_MAX_KEY_WIDTH bytes.
 *
 *    The length of 'values' must be no more than
 *    HASHLIB_MAX_VALUE_WIDTH bytes.
 *
 *    At least one 'values' or one 'distincts' field must be specified.
 */
int
skUniqueSetFields(
    sk_unique_t            *uniq,
    const sk_fieldlist_t   *key_fields,
    const sk_fieldlist_t   *distinct_fields,
    const sk_fieldlist_t   *agg_value_fields);

/*
 *  ok = skUniquePrepareForInput(uniq);
 *
 *    Tell the unique object 'uniq' that initialization is complete,
 *    and that the caller is now ready to begin adding records to the
 *    unique object.
 *
 *    Return an error if the object is not completely specified or if
 *    there are field errors as specified in skUniqueSetFields().
 */
int
skUniquePrepareForInput(
    sk_unique_t        *uniq);

/*
 *  ok = skUniquePrepareForOutput(uniq);
 *
 *    Tell the unique object 'uniq' that all records have been added,
 *    and the unique object can now do whatever it needs to do before
 *    the caller requests get the binned records.
 *
 *    Return 0 on success.  Return -1 if skUniquePrepareForInput() has
 *    not yet been called.
 *
 *    This function may write data to a temporary file; it will return
 *    -1 if there is an error writing the file.
 *
 *    It is an error to call skUniqueAddRecord() after this function
 *    has been called.
 */
int
skUniquePrepareForOutput(
    sk_unique_t        *uniq);

/*
 *  ok = skUniqueAddRecord(uniq, rwrec);
 *
 *    Add the SiLK Flow record 'rwrec' to the unique object 'uniq'.
 *    This function should only be called after a call to
 *    skUniquePrepareForInput().
 *
 *    This function will call skFieldListRecToBinary() for the key
 *    fields (and the distinct fields, if any).  This function will
 *    call skFieldListAddRecToBuffer() for the value fields, if any.
 *
 *    Return 0 on success.  Return -1 on error.  Error conditions
 *    include calling this function before calling
 *    skUniquePrepareForInput(), being unable to write a temporary
 *    file, or being unable to create a new hash table after writing
 *    the current hash table to disk.
 */
int
skUniqueAddRecord(
    sk_unique_t        *uniq,
    rwRec              *rwrec);

/*
 *  ok = skUniqueIteratorCreate(uniq, &iter);
 *
 *    Create a new iterator that can be used to get the bins from the
 *    unique object, 'uniq'.  The iterator is stored in the location
 *    pointed to by 'iter'.
 *
 *    It is an error to call this function before calling
 *    skUniquePrepareForOutput().
 *
 *    Return 0 on success, or -1 if there is an error allocating
 *    memory for the iterator.  This function may affect temporary
 *    files; it will return -1 if there is an error reading or writing
 *    the temporary files.
 */
int
skUniqueIteratorCreate(
    sk_unique_t            *uniq,
    sk_unique_iterator_t  **iter);

/*
 *  skUniqueIteratorDestroy(&iter);
 *
 *    Destroy the iterator pointed to by 'iter' and set 'iter' to
 *    NULL.  The function is a no-op if iter or *iter is NULL.
 */
#define skUniqueIteratorDestroy(uniqit_iter)    \
    if (*(uniqit_iter)) {                       \
        (*(uniqit_iter))->free_fn(uniqit_iter); \
    }

/*
 *  ok = skUniqueIteratorReset(iter);
 *
 *    Reset the iterator 'iter' so it can loop through the entries in
 *    the unique object another time.  May return -1 if there is an
 *    error allocating memory for the iterator.
 */
#define skUniqueIteratorReset(uniqit_iter)      \
    ((uniqit_iter)->reset_fn(uniqit_iter))

/*
 *  ok = skUniqueIteratorNext(iter, &key_fields_buf, &distinct_fields_buf, &value_fields_buf);
 *
 *    Set the pointers referenced by 'key_fields_buf',
 *    'distinct_fields_buf', and 'value_fields_buf' to the location of
 *    the buffers holding values associated with the current row.  The
 *    value remain valid until the next call to this function.
 *
 *    Return SK_ITERATOR_OK if there was another row of data; return
 *    SK_ITERATOR_NO_MORE_ENTRIES when all rows have been visited.
 */
#define skUniqueIteratorNext(uniqit_iter, uniqit_key, uniqit_dis, uniqit_val) \
    ((uniqit_iter)->next_fn((uniqit_iter), (uniqit_key),                \
                            (uniqit_dis), (uniqit_val)))


/*
 *    PRIVATE.  For internal use only.
 *
 *    Specify the signature for the function to destroy an iterator.
 */
typedef void (*sk_uniqiter_free_fn_t)(
    sk_unique_iterator_t          **iter);

/*
 *    PRIVATE.  For internal use only.
 *
 *    Specify the signature for the function to reset an iterator.
 */
typedef int (*sk_uniqiter_reset_fn_t)(
    sk_unique_iterator_t           *iter);

/*
 *    PRIVATE.  For internal use only.
 *
 *    Specify the signature for the function to get values for the
 *    current row from the iterator.
 *
 *    int skUniqueIteratorNext(
 *        sk_unique_iterator_t           *iter,
 *        uint8_t                       **key_fields_buffer,
 *        uint8_t                       **distinct_fields_buffer,
 *        uint8_t                       **agg_value_fields_buffer);
 */
typedef int (*sk_uniqiter_next_fn_t)(
    sk_unique_iterator_t           *iter,
    uint8_t                       **key_fields_buffer,
    uint8_t                       **distinct_fields_buffer,
    uint8_t                       **value_fields_buffer);

/*
 *    PRIVATE.  For internal use only.
 *
 *    typedef struct sk_unique_iterator_st sk_unique_iterator_t;
 *
 *    The internal structure of the iterator is made available to
 *    avoid an extra function call when iterating over the elements of
 *    an sk_unique_t.
 */
struct sk_unique_iterator_st {
    sk_uniqiter_reset_fn_t  reset_fn;
    sk_uniqiter_next_fn_t   next_fn;
    sk_uniqiter_free_fn_t   free_fn;
};



/* ************************************************************ */
/* ************************************************************ */

/*
 *    The sk_sort_unique_t structure is similar to sk_unique_t;
 *    however, sk_sort_unique_t is specifically designed to handle
 *    pre-sorted input.
 *
 *    For some types of calculations (particularly summing the number
 *    of packets or bytes), knowing that the data is presorted allows
 *    fast calculations since there is no need to create a hash table.
 *    When computing a count of distinct values, using sk_unique_t can
 *    be faster than using sk_sort_unique_t, since sk_sort_unique_t
 *    will still create a hash table to count the distinct values.
 *
 *    The processing of sk_sort_unique_t is very different from that
 *    for sk_unique_t.  sk_sort_unique_t processing is callback based,
 *    as specified here:
 *
 *      skPresortedUniqueCreate(&u);
 *      skPresortedUniqueSetFields(u, key, distinct, value);
 *      skPresortedUniqueSetTempDirectory(u);        //optional
 *      skPresortedUniqueSetReadFn(u, read_cb);      //optional
 *      skPresortedUniqueSetFlowIterator(rec, flowiter);
 *      skPresortedUniqueProcess(u, output_cb);
 *      skPresortedUniqueDestroy(&u);
 *
 *    where the 'output_cb' callback is called for each unique bin.
 *
 */
typedef struct sk_sort_unique_st sk_sort_unique_t;

/*
 *  ok = skPresortedUniqueCreate(&ps_uniq);
 *
 *    Create a new unique object and store its address in the parameter
 *    'ps_uniq'.  Return -1 if the unique object cannot be created.
 */
int
skPresortedUniqueCreate(
    sk_sort_unique_t  **ps_uniq);

/*
 *  skPresortedUniqueDestroy(&ps_uniq);
 *
 *    Destroy the unique object whose address is specified in 'ps_uniq'.
 *    Remove any temporary files.  This function may be called with a
 *    NULL pointer.
 */
void
skPresortedUniqueDestroy(
    sk_sort_unique_t  **ps_uniq);

/*
 *  skPresortedUniqueSetTempDirectory(ps_uniq, temp_dir);
 *
 *    Specify that the unique object 'ps_uniq' should use the
 *    directory 'temp_dir' to store temporary files.  This function
 *    makes a copy of the parameter 'temp_dir'.
 */
void
skPresortedUniqueSetTempDirectory(
    sk_sort_unique_t   *ps_uniq,
    const char         *temp_dir);

/*
 *  ok = skPresortedUniqueSetFields(ps_uniq, keys, distincts, values);
 *
 *    Specify the fields that the unique object 'ps_uniq' should use.
 *    It is an error to call this function once
 *    skPresortedUniqueProcess() has been called.
 *
 *    'keys' is a field list containing the fields to bin by.  It must
 *    contain at least one field.
 *
 *    'distincts' is a field list containing the fields where the
 *    caller wishes to get a count of distinct values per key.  It may
 *    be NULL or contain no fields, subject to the additional
 *    constraints given below.  The keys specified in 'distincts' may
 *    not be present in 'keys'.
 *
 *    'values' is a field list containing the fields where the caller
 *    wishes to compute an aggregate value per key.  It may be NULL or
 *    contain no fields, subject to the additional containsts given
 *    below.
 *
 *    If this function is called multiple times, the previous field
 *    values will be lost.
 *
 *    There are additional constaints, which will be checked when
 *    skPresortedUniqueProcess() is called:
 *
 *    The length of 'keys' and 'distincts' must total to no more than
 *    HASHLIB_MAX_KEY_WIDTH bytes.
 *
 *    The length of 'values' must be no more than
 *    HASHLIB_MAX_VALUE_WIDTH bytes.
 *
 *    At least one 'values' or one 'distincts' field must be specified.
 */
int
skPresortedUniqueSetFields(
    sk_sort_unique_t       *ps_uniq,
    const sk_fieldlist_t   *key_fields,
    const sk_fieldlist_t   *distinct_fields,
    const sk_fieldlist_t   *agg_value_fields);

/*
 *  ok = skPresortedUniqueSetFlowIterator(ps_uniq, flowiter);
 *
 *    Specify that 'ps_uniq' should process the SiLK Flow streams that
 *    the flow iterator 'flowiter' holds.  'ps_uniq' assumes the
 *    records in each of those streams have been sorted using the same
 *    key as specified in the call to skPresortedUniqueSetFields().
 *
 *    The function returns 0 on success, or -1 if there was an error
 *    allocating memory.
 *
 *    It is an error to call this function once
 *    skPresortedUniqueProcess() has been called.
 */
int
skPresortedUniqueSetFlowIterator(
    sk_sort_unique_t   *uniq,
    sk_flow_iter_t     *flowiter);

/*
 *  ok = skPresortedUniqueSetReadFn(ps_uniq, read_rec_func);
 *
 *    Specify that the 'read_rec_func' should be called to read a
 *    record from an open skstream_t.  If this function is not called,
 *    or if the 'read_rec_func' is NULL, the ps_uniq object uses
 *    skStreamReadRecord() to get a record from the stream.
 *
 *    It is an error to call this function once
 *    skPresortedUniqueProcess() has been called.
 */
int
skPresortedUniqueSetReadFn(
    sk_sort_unique_t   *ps_uniq,
    int               (*stream_read)(skstream_t *, rwRec *));

/*
 *    The sk_unique_output_fn_t specifies the signature of the
 *    callback function that the skPresortedUniqueProcess() function
 *    will invoke for each unique bin that it finds.
 *
 *    This function should print the result or do whatever other
 *    processing is required.
 *
 *    The 'key_fields_buffer', 'distinct_fields_buffer', and
 *    'value_fields_buffer' will contain the buffers for the
 *    key_fields, distinct_fields, and value_fields specified to
 *    skPresortedUniqueSetFields().  Those buffers are valid until the
 *    function returns.  If distinct_fields or value_fields were not
 *    specified, the corresponding buffer will be NULL.
 *
 *    The 'callback_data' parameter is a user-specified value passed
 *    to the skPresortedUniqueProcess() function.  It may be NULL.
 *    The callback does not automatically get a handle to the
 *    'sk_sort_unique_t' object; the caller may wish to specify that
 *    object as (part of) the callback_data.
 */
typedef int (*sk_unique_output_fn_t)(
    const uint8_t                  *key_fields_buffer,
    const uint8_t                  *distinct_fields_buffer,
    const uint8_t                  *value_fields_buffer,
    void                           *callback_data);

/*
 *  ok = skPresortedUniqueProcess(ps_uniq, output_fn, callback_data);
 *
 *    Tell 'ps_uniq' that it should process the input files that were
 *    specified in the call to skPresortedUniqueSetFlowIterator().
 *
 *    Records are read from the files using the 'read_rec_func'
 *    callback, or skStreamReadRecord() if that callback is NULL.
 *
 *    When 'ps_uniq' determines that a bin is complete, it will invoke
 *    the 'output_fn' specified here.  Note that the buffers that
 *    skPresortedUniqueProcess() hands to the callback are valid until
 *    the callback returns.  If the 'output_fn' returns a non-zero
 *    value, processing stops.
 *
 *    The 'callback_data' parameter is a user-specified value that
 *    gets passed to each invocation of the 'output_fn'; this
 *    parameter may be NULL.
 *
 *    The function returns 0 once all records have read, or -1 if
 *    there is an error during processing.
 */
int
skPresortedUniqueProcess(
    sk_sort_unique_t       *ps_uniq,
    sk_unique_output_fn_t   output_fn,
    void                   *callback_data);

#ifdef __cplusplus
}
#endif
#endif  /* _SKUNIQUE_H */

/*
** Local Variables:
** mode:c
** indent-tabs-mode:nil
** c-basic-offset:4
** End:
*/
