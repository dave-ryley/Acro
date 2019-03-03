// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: Main.capnp

#pragma once

#include <capnp/generated-header-support.h>
#include <kj/windows-sanity.h>
#if !CAPNP_LITE
#include <capnp/capability.h>
#endif  // !CAPNP_LITE

#if CAPNP_VERSION != 7000
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif

#include "Acro.capnp.h"
#include "ObjectBroker.capnp.h"

namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(a7a589962cecf840);
CAPNP_DECLARE_SCHEMA(98052bafeb9cf4f1);
CAPNP_DECLARE_SCHEMA(b7fe892aa0897508);
CAPNP_DECLARE_SCHEMA(ea990da4cec8ac99);
CAPNP_DECLARE_SCHEMA(ec9e9cc8f75a238b);
CAPNP_DECLARE_SCHEMA(e7ad0271d3efc35b);
CAPNP_DECLARE_SCHEMA(f1d45d4ca266f252);

}  // namespace schemas
}  // namespace capnp


struct Main {
  Main() = delete;

#if !CAPNP_LITE
  class Client;
  class Server;
#endif  // !CAPNP_LITE

  struct HealthCheckParams;
  struct HealthCheckResults;
  struct GetAcroParams;
  struct GetAcroResults;
  struct GetObjectBrokerParams;
  struct GetObjectBrokerResults;

  #if !CAPNP_LITE
  struct _capnpPrivate {
    CAPNP_DECLARE_INTERFACE_HEADER(a7a589962cecf840)
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
  };
  #endif  // !CAPNP_LITE
};

struct Main::HealthCheckParams {
  HealthCheckParams() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(98052bafeb9cf4f1, 0, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Main::HealthCheckResults {
  HealthCheckResults() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(b7fe892aa0897508, 1, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Main::GetAcroParams {
  GetAcroParams() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(ea990da4cec8ac99, 0, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Main::GetAcroResults {
  GetAcroResults() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(ec9e9cc8f75a238b, 0, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Main::GetObjectBrokerParams {
  GetObjectBrokerParams() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(e7ad0271d3efc35b, 0, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Main::GetObjectBrokerResults {
  GetObjectBrokerResults() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(f1d45d4ca266f252, 0, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

#if !CAPNP_LITE
class Main::Client
    : public virtual ::capnp::Capability::Client {
public:
  typedef Main Calls;
  typedef Main Reads;

  Client(decltype(nullptr));
  explicit Client(::kj::Own< ::capnp::ClientHook>&& hook);
  template <typename _t, typename = ::kj::EnableIf< ::kj::canConvert<_t*, Server*>()>>
  Client(::kj::Own<_t>&& server);
  template <typename _t, typename = ::kj::EnableIf< ::kj::canConvert<_t*, Client*>()>>
  Client(::kj::Promise<_t>&& promise);
  Client(::kj::Exception&& exception);
  Client(Client&) = default;
  Client(Client&&) = default;
  Client& operator=(Client& other);
  Client& operator=(Client&& other);

  ::capnp::Request< ::Main::HealthCheckParams,  ::Main::HealthCheckResults> healthCheckRequest(
      ::kj::Maybe< ::capnp::MessageSize> sizeHint = nullptr);
  ::capnp::Request< ::Main::GetAcroParams,  ::Main::GetAcroResults> getAcroRequest(
      ::kj::Maybe< ::capnp::MessageSize> sizeHint = nullptr);
  ::capnp::Request< ::Main::GetObjectBrokerParams,  ::Main::GetObjectBrokerResults> getObjectBrokerRequest(
      ::kj::Maybe< ::capnp::MessageSize> sizeHint = nullptr);

protected:
  Client() = default;
};

class Main::Server
    : public virtual ::capnp::Capability::Server {
public:
  typedef Main Serves;

  ::kj::Promise<void> dispatchCall(uint64_t interfaceId, uint16_t methodId,
      ::capnp::CallContext< ::capnp::AnyPointer, ::capnp::AnyPointer> context)
      override;

protected:
  typedef  ::Main::HealthCheckParams HealthCheckParams;
  typedef  ::Main::HealthCheckResults HealthCheckResults;
  typedef ::capnp::CallContext<HealthCheckParams, HealthCheckResults> HealthCheckContext;
  virtual ::kj::Promise<void> healthCheck(HealthCheckContext context);
  typedef  ::Main::GetAcroParams GetAcroParams;
  typedef  ::Main::GetAcroResults GetAcroResults;
  typedef ::capnp::CallContext<GetAcroParams, GetAcroResults> GetAcroContext;
  virtual ::kj::Promise<void> getAcro(GetAcroContext context);
  typedef  ::Main::GetObjectBrokerParams GetObjectBrokerParams;
  typedef  ::Main::GetObjectBrokerResults GetObjectBrokerResults;
  typedef ::capnp::CallContext<GetObjectBrokerParams, GetObjectBrokerResults> GetObjectBrokerContext;
  virtual ::kj::Promise<void> getObjectBroker(GetObjectBrokerContext context);

  inline  ::Main::Client thisCap() {
    return ::capnp::Capability::Server::thisCap()
        .template castAs< ::Main>();
  }

  ::kj::Promise<void> dispatchCallInternal(uint16_t methodId,
      ::capnp::CallContext< ::capnp::AnyPointer, ::capnp::AnyPointer> context);
};
#endif  // !CAPNP_LITE

class Main::HealthCheckParams::Reader {
public:
  typedef HealthCheckParams Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Main::HealthCheckParams::Builder {
public:
  typedef HealthCheckParams Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Main::HealthCheckParams::Pipeline {
public:
  typedef HealthCheckParams Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Main::HealthCheckResults::Reader {
public:
  typedef HealthCheckResults Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool getResult() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Main::HealthCheckResults::Builder {
public:
  typedef HealthCheckResults Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool getResult();
  inline void setResult(bool value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Main::HealthCheckResults::Pipeline {
public:
  typedef HealthCheckResults Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Main::GetAcroParams::Reader {
public:
  typedef GetAcroParams Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Main::GetAcroParams::Builder {
public:
  typedef GetAcroParams Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Main::GetAcroParams::Pipeline {
public:
  typedef GetAcroParams Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Main::GetAcroResults::Reader {
public:
  typedef GetAcroResults Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasAcro() const;
#if !CAPNP_LITE
  inline  ::Acro::Client getAcro() const;
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Main::GetAcroResults::Builder {
public:
  typedef GetAcroResults Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasAcro();
#if !CAPNP_LITE
  inline  ::Acro::Client getAcro();
  inline void setAcro( ::Acro::Client&& value);
  inline void setAcro( ::Acro::Client& value);
  inline void adoptAcro(::capnp::Orphan< ::Acro>&& value);
  inline ::capnp::Orphan< ::Acro> disownAcro();
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Main::GetAcroResults::Pipeline {
public:
  typedef GetAcroResults Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

  inline  ::Acro::Client getAcro();
private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Main::GetObjectBrokerParams::Reader {
public:
  typedef GetObjectBrokerParams Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Main::GetObjectBrokerParams::Builder {
public:
  typedef GetObjectBrokerParams Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Main::GetObjectBrokerParams::Pipeline {
public:
  typedef GetObjectBrokerParams Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Main::GetObjectBrokerResults::Reader {
public:
  typedef GetObjectBrokerResults Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasObjectBroker() const;
#if !CAPNP_LITE
  inline  ::Gorb::ObjectBroker::Client getObjectBroker() const;
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Main::GetObjectBrokerResults::Builder {
public:
  typedef GetObjectBrokerResults Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasObjectBroker();
#if !CAPNP_LITE
  inline  ::Gorb::ObjectBroker::Client getObjectBroker();
  inline void setObjectBroker( ::Gorb::ObjectBroker::Client&& value);
  inline void setObjectBroker( ::Gorb::ObjectBroker::Client& value);
  inline void adoptObjectBroker(::capnp::Orphan< ::Gorb::ObjectBroker>&& value);
  inline ::capnp::Orphan< ::Gorb::ObjectBroker> disownObjectBroker();
#endif  // !CAPNP_LITE

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Main::GetObjectBrokerResults::Pipeline {
public:
  typedef GetObjectBrokerResults Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

  inline  ::Gorb::ObjectBroker::Client getObjectBroker();
private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

// =======================================================================================

#if !CAPNP_LITE
inline Main::Client::Client(decltype(nullptr))
    : ::capnp::Capability::Client(nullptr) {}
inline Main::Client::Client(
    ::kj::Own< ::capnp::ClientHook>&& hook)
    : ::capnp::Capability::Client(::kj::mv(hook)) {}
template <typename _t, typename>
inline Main::Client::Client(::kj::Own<_t>&& server)
    : ::capnp::Capability::Client(::kj::mv(server)) {}
template <typename _t, typename>
inline Main::Client::Client(::kj::Promise<_t>&& promise)
    : ::capnp::Capability::Client(::kj::mv(promise)) {}
inline Main::Client::Client(::kj::Exception&& exception)
    : ::capnp::Capability::Client(::kj::mv(exception)) {}
inline  ::Main::Client& Main::Client::operator=(Client& other) {
  ::capnp::Capability::Client::operator=(other);
  return *this;
}
inline  ::Main::Client& Main::Client::operator=(Client&& other) {
  ::capnp::Capability::Client::operator=(kj::mv(other));
  return *this;
}

#endif  // !CAPNP_LITE
inline bool Main::HealthCheckResults::Reader::getResult() const {
  return _reader.getDataField<bool>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline bool Main::HealthCheckResults::Builder::getResult() {
  return _builder.getDataField<bool>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void Main::HealthCheckResults::Builder::setResult(bool value) {
  _builder.setDataField<bool>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

inline bool Main::GetAcroResults::Reader::hasAcro() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool Main::GetAcroResults::Builder::hasAcro() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
#if !CAPNP_LITE
inline  ::Acro::Client Main::GetAcroResults::Reader::getAcro() const {
  return ::capnp::_::PointerHelpers< ::Acro>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::Acro::Client Main::GetAcroResults::Builder::getAcro() {
  return ::capnp::_::PointerHelpers< ::Acro>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::Acro::Client Main::GetAcroResults::Pipeline::getAcro() {
  return  ::Acro::Client(_typeless.getPointerField(0).asCap());
}
inline void Main::GetAcroResults::Builder::setAcro( ::Acro::Client&& cap) {
  ::capnp::_::PointerHelpers< ::Acro>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(cap));
}
inline void Main::GetAcroResults::Builder::setAcro( ::Acro::Client& cap) {
  ::capnp::_::PointerHelpers< ::Acro>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), cap);
}
inline void Main::GetAcroResults::Builder::adoptAcro(
    ::capnp::Orphan< ::Acro>&& value) {
  ::capnp::_::PointerHelpers< ::Acro>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::Acro> Main::GetAcroResults::Builder::disownAcro() {
  return ::capnp::_::PointerHelpers< ::Acro>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
#endif  // !CAPNP_LITE

inline bool Main::GetObjectBrokerResults::Reader::hasObjectBroker() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool Main::GetObjectBrokerResults::Builder::hasObjectBroker() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
#if !CAPNP_LITE
inline  ::Gorb::ObjectBroker::Client Main::GetObjectBrokerResults::Reader::getObjectBroker() const {
  return ::capnp::_::PointerHelpers< ::Gorb::ObjectBroker>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::Gorb::ObjectBroker::Client Main::GetObjectBrokerResults::Builder::getObjectBroker() {
  return ::capnp::_::PointerHelpers< ::Gorb::ObjectBroker>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::Gorb::ObjectBroker::Client Main::GetObjectBrokerResults::Pipeline::getObjectBroker() {
  return  ::Gorb::ObjectBroker::Client(_typeless.getPointerField(0).asCap());
}
inline void Main::GetObjectBrokerResults::Builder::setObjectBroker( ::Gorb::ObjectBroker::Client&& cap) {
  ::capnp::_::PointerHelpers< ::Gorb::ObjectBroker>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(cap));
}
inline void Main::GetObjectBrokerResults::Builder::setObjectBroker( ::Gorb::ObjectBroker::Client& cap) {
  ::capnp::_::PointerHelpers< ::Gorb::ObjectBroker>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), cap);
}
inline void Main::GetObjectBrokerResults::Builder::adoptObjectBroker(
    ::capnp::Orphan< ::Gorb::ObjectBroker>&& value) {
  ::capnp::_::PointerHelpers< ::Gorb::ObjectBroker>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::Gorb::ObjectBroker> Main::GetObjectBrokerResults::Builder::disownObjectBroker() {
  return ::capnp::_::PointerHelpers< ::Gorb::ObjectBroker>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
#endif  // !CAPNP_LITE

