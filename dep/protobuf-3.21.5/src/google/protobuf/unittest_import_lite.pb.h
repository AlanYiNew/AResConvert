// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_import_lite.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5fimport_5flite_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5fimport_5flite_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021005 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_util.h>
#include "google/protobuf/unittest_import_public_lite.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_google_2fprotobuf_2funittest_5fimport_5flite_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_google_2fprotobuf_2funittest_5fimport_5flite_2eproto {
  static const uint32_t offsets[];
};
namespace protobuf_unittest_import {
class ImportMessageLite;
struct ImportMessageLiteDefaultTypeInternal;
extern ImportMessageLiteDefaultTypeInternal _ImportMessageLite_default_instance_;
}  // namespace protobuf_unittest_import
PROTOBUF_NAMESPACE_OPEN
template<> ::protobuf_unittest_import::ImportMessageLite* Arena::CreateMaybeMessage<::protobuf_unittest_import::ImportMessageLite>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace protobuf_unittest_import {

enum ImportEnumLite : int {
  IMPORT_LITE_FOO = 7,
  IMPORT_LITE_BAR = 8,
  IMPORT_LITE_BAZ = 9
};
bool ImportEnumLite_IsValid(int value);
constexpr ImportEnumLite ImportEnumLite_MIN = IMPORT_LITE_FOO;
constexpr ImportEnumLite ImportEnumLite_MAX = IMPORT_LITE_BAZ;
constexpr int ImportEnumLite_ARRAYSIZE = ImportEnumLite_MAX + 1;

const std::string& ImportEnumLite_Name(ImportEnumLite value);
template<typename T>
inline const std::string& ImportEnumLite_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ImportEnumLite>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ImportEnumLite_Name.");
  return ImportEnumLite_Name(static_cast<ImportEnumLite>(enum_t_value));
}
bool ImportEnumLite_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ImportEnumLite* value);
// ===================================================================

class ImportMessageLite final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:protobuf_unittest_import.ImportMessageLite) */ {
 public:
  inline ImportMessageLite() : ImportMessageLite(nullptr) {}
  ~ImportMessageLite() override;
  explicit PROTOBUF_CONSTEXPR ImportMessageLite(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ImportMessageLite(const ImportMessageLite& from);
  ImportMessageLite(ImportMessageLite&& from) noexcept
    : ImportMessageLite() {
    *this = ::std::move(from);
  }

  inline ImportMessageLite& operator=(const ImportMessageLite& from) {
    CopyFrom(from);
    return *this;
  }
  inline ImportMessageLite& operator=(ImportMessageLite&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString);
  }
  inline std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<std::string>();
  }

  static const ImportMessageLite& default_instance() {
    return *internal_default_instance();
  }
  static inline const ImportMessageLite* internal_default_instance() {
    return reinterpret_cast<const ImportMessageLite*>(
               &_ImportMessageLite_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(ImportMessageLite& a, ImportMessageLite& b) {
    a.Swap(&b);
  }
  inline void Swap(ImportMessageLite* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ImportMessageLite* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ImportMessageLite* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ImportMessageLite>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const ImportMessageLite& from);
  void MergeFrom(const ImportMessageLite& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(ImportMessageLite* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "protobuf_unittest_import.ImportMessageLite";
  }
  protected:
  explicit ImportMessageLite(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kDFieldNumber = 1,
  };
  // optional int32 d = 1;
  bool has_d() const;
  private:
  bool _internal_has_d() const;
  public:
  void clear_d();
  int32_t d() const;
  void set_d(int32_t value);
  private:
  int32_t _internal_d() const;
  void _internal_set_d(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:protobuf_unittest_import.ImportMessageLite)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
    int32_t d_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_google_2fprotobuf_2funittest_5fimport_5flite_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ImportMessageLite

// optional int32 d = 1;
inline bool ImportMessageLite::_internal_has_d() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool ImportMessageLite::has_d() const {
  return _internal_has_d();
}
inline void ImportMessageLite::clear_d() {
  _impl_.d_ = 0;
  _impl_._has_bits_[0] &= ~0x00000001u;
}
inline int32_t ImportMessageLite::_internal_d() const {
  return _impl_.d_;
}
inline int32_t ImportMessageLite::d() const {
  // @@protoc_insertion_point(field_get:protobuf_unittest_import.ImportMessageLite.d)
  return _internal_d();
}
inline void ImportMessageLite::_internal_set_d(int32_t value) {
  _impl_._has_bits_[0] |= 0x00000001u;
  _impl_.d_ = value;
}
inline void ImportMessageLite::set_d(int32_t value) {
  _internal_set_d(value);
  // @@protoc_insertion_point(field_set:protobuf_unittest_import.ImportMessageLite.d)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace protobuf_unittest_import

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::protobuf_unittest_import::ImportEnumLite> : ::std::true_type {};

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5fimport_5flite_2eproto