// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: action_definitions.proto

#include "action_definitions.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace ActionTracer {
PROTOBUF_CONSTEXPR ActionDataPackage::ActionDataPackage(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_._has_bits_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}
  , /*decltype(_impl_.data_)*/{}
  , /*decltype(_impl_.device_identifier_contents_)*/0u} {}
struct ActionDataPackageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR ActionDataPackageDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~ActionDataPackageDefaultTypeInternal() {}
  union {
    ActionDataPackage _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 ActionDataPackageDefaultTypeInternal _ActionDataPackage_default_instance_;
}  // namespace ActionTracer
static ::_pb::Metadata file_level_metadata_action_5fdefinitions_2eproto[1];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_action_5fdefinitions_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_action_5fdefinitions_2eproto = nullptr;

const uint32_t TableStruct_action_5fdefinitions_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::ActionTracer::ActionDataPackage, _impl_._has_bits_),
  PROTOBUF_FIELD_OFFSET(::ActionTracer::ActionDataPackage, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::ActionTracer::ActionDataPackage, _impl_.device_identifier_contents_),
  PROTOBUF_FIELD_OFFSET(::ActionTracer::ActionDataPackage, _impl_.data_),
  0,
  ~0u,
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 8, -1, sizeof(::ActionTracer::ActionDataPackage)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::ActionTracer::_ActionDataPackage_default_instance_._instance,
};

const char descriptor_table_protodef_action_5fdefinitions_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\030action_definitions.proto\022\014ActionTracer"
  "\"E\n\021ActionDataPackage\022\"\n\032device_identifi"
  "er_contents\030\001 \002(\r\022\014\n\004data\030\002 \003(\002"
  ;
static ::_pbi::once_flag descriptor_table_action_5fdefinitions_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_action_5fdefinitions_2eproto = {
    false, false, 111, descriptor_table_protodef_action_5fdefinitions_2eproto,
    "action_definitions.proto",
    &descriptor_table_action_5fdefinitions_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_action_5fdefinitions_2eproto::offsets,
    file_level_metadata_action_5fdefinitions_2eproto, file_level_enum_descriptors_action_5fdefinitions_2eproto,
    file_level_service_descriptors_action_5fdefinitions_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_action_5fdefinitions_2eproto_getter() {
  return &descriptor_table_action_5fdefinitions_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_action_5fdefinitions_2eproto(&descriptor_table_action_5fdefinitions_2eproto);
namespace ActionTracer {

// ===================================================================

class ActionDataPackage::_Internal {
 public:
  using HasBits = decltype(std::declval<ActionDataPackage>()._impl_._has_bits_);
  static void set_has_device_identifier_contents(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static bool MissingRequiredFields(const HasBits& has_bits) {
    return ((has_bits[0] & 0x00000001) ^ 0x00000001) != 0;
  }
};

ActionDataPackage::ActionDataPackage(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:ActionTracer.ActionDataPackage)
}
ActionDataPackage::ActionDataPackage(const ActionDataPackage& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  ActionDataPackage* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.data_){from._impl_.data_}
    , decltype(_impl_.device_identifier_contents_){}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _this->_impl_.device_identifier_contents_ = from._impl_.device_identifier_contents_;
  // @@protoc_insertion_point(copy_constructor:ActionTracer.ActionDataPackage)
}

inline void ActionDataPackage::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.data_){arena}
    , decltype(_impl_.device_identifier_contents_){0u}
  };
}

ActionDataPackage::~ActionDataPackage() {
  // @@protoc_insertion_point(destructor:ActionTracer.ActionDataPackage)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void ActionDataPackage::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.data_.~RepeatedField();
}

void ActionDataPackage::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void ActionDataPackage::Clear() {
// @@protoc_insertion_point(message_clear_start:ActionTracer.ActionDataPackage)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.data_.Clear();
  _impl_.device_identifier_contents_ = 0u;
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* ActionDataPackage::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // required uint32 device_identifier_contents = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _Internal::set_has_device_identifier_contents(&has_bits);
          _impl_.device_identifier_contents_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // repeated float data = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 21)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_data(::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr));
            ptr += sizeof(float);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<21>(ptr));
        } else if (static_cast<uint8_t>(tag) == 18) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedFloatParser(_internal_mutable_data(), ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _impl_._has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* ActionDataPackage::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:ActionTracer.ActionDataPackage)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  // required uint32 device_identifier_contents = 1;
  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(1, this->_internal_device_identifier_contents(), target);
  }

  // repeated float data = 2;
  for (int i = 0, n = this->_internal_data_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(2, this->_internal_data(i), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ActionTracer.ActionDataPackage)
  return target;
}

size_t ActionDataPackage::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:ActionTracer.ActionDataPackage)
  size_t total_size = 0;

  // required uint32 device_identifier_contents = 1;
  if (_internal_has_device_identifier_contents()) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_device_identifier_contents());
  }
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated float data = 2;
  {
    unsigned int count = static_cast<unsigned int>(this->_internal_data_size());
    size_t data_size = 4UL * count;
    total_size += 1 *
                  ::_pbi::FromIntSize(this->_internal_data_size());
    total_size += data_size;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData ActionDataPackage::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    ActionDataPackage::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*ActionDataPackage::GetClassData() const { return &_class_data_; }


void ActionDataPackage::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<ActionDataPackage*>(&to_msg);
  auto& from = static_cast<const ActionDataPackage&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:ActionTracer.ActionDataPackage)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_impl_.data_.MergeFrom(from._impl_.data_);
  if (from._internal_has_device_identifier_contents()) {
    _this->_internal_set_device_identifier_contents(from._internal_device_identifier_contents());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void ActionDataPackage::CopyFrom(const ActionDataPackage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ActionTracer.ActionDataPackage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ActionDataPackage::IsInitialized() const {
  if (_Internal::MissingRequiredFields(_impl_._has_bits_)) return false;
  return true;
}

void ActionDataPackage::InternalSwap(ActionDataPackage* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  _impl_.data_.InternalSwap(&other->_impl_.data_);
  swap(_impl_.device_identifier_contents_, other->_impl_.device_identifier_contents_);
}

::PROTOBUF_NAMESPACE_ID::Metadata ActionDataPackage::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_action_5fdefinitions_2eproto_getter, &descriptor_table_action_5fdefinitions_2eproto_once,
      file_level_metadata_action_5fdefinitions_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace ActionTracer
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::ActionTracer::ActionDataPackage*
Arena::CreateMaybeMessage< ::ActionTracer::ActionDataPackage >(Arena* arena) {
  return Arena::CreateMessageInternal< ::ActionTracer::ActionDataPackage >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
