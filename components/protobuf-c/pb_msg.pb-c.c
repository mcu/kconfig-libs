/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: pb_msg.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "pb_msg.pb-c.h"
void   pb__msg__init
                     (Pb__Msg         *message)
{
  static const Pb__Msg init_value = PB__MSG__INIT;
  *message = init_value;
}
size_t pb__msg__get_packed_size
                     (const Pb__Msg *message)
{
  assert(message->base.descriptor == &pb__msg__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t pb__msg__pack
                     (const Pb__Msg *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &pb__msg__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t pb__msg__pack_to_buffer
                     (const Pb__Msg *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &pb__msg__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Pb__Msg *
       pb__msg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Pb__Msg *)
     protobuf_c_message_unpack (&pb__msg__descriptor,
                                allocator, len, data);
}
void   pb__msg__free_unpacked
                     (Pb__Msg *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &pb__msg__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor pb__msg__field_descriptors[5] =
{
  {
    "cmd_id",
    45,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Pb__Msg, cmd_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "string",
    161,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Pb__Msg, string),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "err_id",
    180,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_UINT32,
    offsetof(Pb__Msg, has_err_id),
    offsetof(Pb__Msg, err_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "number",
    201,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_FLOAT,
    offsetof(Pb__Msg, has_number),
    offsetof(Pb__Msg, number),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "msg_id",
    245,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Pb__Msg, msg_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned pb__msg__field_indices_by_name[] = {
  0,   /* field[0] = cmd_id */
  2,   /* field[2] = err_id */
  4,   /* field[4] = msg_id */
  3,   /* field[3] = number */
  1,   /* field[1] = string */
};
static const ProtobufCIntRange pb__msg__number_ranges[5 + 1] =
{
  { 45, 0 },
  { 161, 1 },
  { 180, 2 },
  { 201, 3 },
  { 245, 4 },
  { 0, 5 }
};
const ProtobufCMessageDescriptor pb__msg__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "pb.msg",
  "Msg",
  "Pb__Msg",
  "pb",
  sizeof(Pb__Msg),
  5,
  pb__msg__field_descriptors,
  pb__msg__field_indices_by_name,
  5,  pb__msg__number_ranges,
  (ProtobufCMessageInit) pb__msg__init,
  NULL,NULL,NULL    /* reserved[123] */
};
