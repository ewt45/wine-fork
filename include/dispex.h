/*** Autogenerated by WIDL 0.1 from dispex.idl - Do not edit ***/
#include <rpc.h>
#include <rpcndr.h>

#ifndef __WIDL_DISPEX_H
#define __WIDL_DISPEX_H
#ifdef __cplusplus
extern "C" {
#endif
#include <ocidl.h>
#include <oleidl.h>
#include <oaidl.h>
#include <servprov.h>
#define fdexNameCaseSensitive       0x00000001L
#define fdexNameEnsure              0x00000002L
#define fdexNameImplicit            0x00000004L
#define fdexNameCaseInsensitive     0x00000008L
#define fdexNameInternal            0x00000010L
#define fdexNameNoDynamicProperties 0x00000020L
#define fdexPropCanGet              0x00000001L
#define fdexPropCannotGet           0x00000002L
#define fdexPropCanPut              0x00000004L
#define fdexPropCannotPut           0x00000008L
#define fdexPropCanPutRef           0x00000010L
#define fdexPropCannotPutRef        0x00000020L
#define fdexPropNoSideEffects       0x00000040L
#define fdexPropDynamicType         0x00000080L
#define fdexPropCanCall             0x00000100L
#define fdexPropCannotCall          0x00000200L
#define fdexPropCanConstruct        0x00000400L
#define fdexPropCannotConstruct     0x00000800L
#define fdexPropCanSourceEvents     0x00001000L
#define fdexPropCannotSourceEvents  0x00002000L
#define fdexEnumDefault             0x00000001L
#define fdexEnumAll                 0x00000002L
#define grfdexPropCanAll \
       (fdexPropCanGet | fdexPropCanPut | fdexPropCanPutRef | \
        fdexPropCanCall | fdexPropCanConstruct | fdexPropCanSourceEvents)
#define grfdexPropCannotAll \
       (fdexPropCannotGet | fdexPropCannotPut | fdexPropCannotPutRef | \
        fdexPropCannotCall | fdexPropCannotConstruct | fdexPropCannotSourceEvents)
#define grfdexPropExtraAll \
       (fdexPropNoSideEffects | fdexPropDynamicType)
#define grfdexPropAll \
       (grfdexPropCanAll | grfdexPropCannotAll | grfdexPropExtraAll)
#define DISPATCH_CONSTRUCT 0x4000
#define DISPID_THIS (-613)
#define DISPID_STARTENUM DISPID_UNKNOWN
#ifndef __IDispatchEx_FWD_DEFINED__
#define __IDispatchEx_FWD_DEFINED__
typedef struct IDispatchEx IDispatchEx;
#endif

/*****************************************************************************
 * IDispatchEx interface
 */
#ifndef __IDispatchEx_INTERFACE_DEFINED__
#define __IDispatchEx_INTERFACE_DEFINED__

DEFINE_GUID(IID_IDispatchEx, 0xa6ef9860, 0xc720, 0x11d0, 0x93,0x37, 0x00,0xa0,0xc9,0x0d,0xca,0xa9);
#if defined(__cplusplus) && !defined(CINTERFACE)
struct IDispatchEx : public IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE GetDispID(
        BSTR bstrName,
        DWORD grfdex,
        DISPID* pid) = 0;

    virtual HRESULT STDMETHODCALLTYPE InvokeEx(
        DISPID id,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS* pdp,
        VARIANT* pvarRes,
        EXCEPINFO* pei,
        IServiceProvider* pspCaller) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteMemberByName(
        BSTR bstrName,
        DWORD grfdex) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteMemberByDispID(
        DISPID id) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetMemberProperties(
        DISPID id,
        DWORD grfdexFetch,
        DWORD* pgrfdex) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetMemberName(
        DISPID id,
        BSTR* pbstrName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNextDispID(
        DWORD grfdex,
        DISPID id,
        DISPID* pid) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNameSpaceParent(
        IUnknown** ppunk) = 0;

};
#else
typedef struct IDispatchExVtbl IDispatchExVtbl;
struct IDispatchEx {
    const IDispatchExVtbl* lpVtbl;
};
struct IDispatchExVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IDispatchEx* This,
        REFIID riid,
        void** ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IDispatchEx* This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IDispatchEx* This);

    /*** IDispatch methods ***/
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(
        IDispatchEx* This,
        UINT* pctinfo);

    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(
        IDispatchEx* This,
        UINT iTInfo,
        LCID lcid,
        ITypeInfo** ppTInfo);

    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(
        IDispatchEx* This,
        REFIID riid,
        LPOLESTR* rgszNames,
        UINT cNames,
        LCID lcid,
        DISPID* rgDispId);

    HRESULT (STDMETHODCALLTYPE *Invoke)(
        IDispatchEx* This,
        DISPID dispIdMember,
        REFIID riid,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS* pDispParams,
        VARIANT* pVarResult,
        EXCEPINFO* pExcepInfo,
        UINT* puArgErr);

    /*** IDispatchEx methods ***/
    HRESULT (STDMETHODCALLTYPE *GetDispID)(
        IDispatchEx* This,
        BSTR bstrName,
        DWORD grfdex,
        DISPID* pid);

    HRESULT (STDMETHODCALLTYPE *InvokeEx)(
        IDispatchEx* This,
        DISPID id,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS* pdp,
        VARIANT* pvarRes,
        EXCEPINFO* pei,
        IServiceProvider* pspCaller);

    HRESULT (STDMETHODCALLTYPE *DeleteMemberByName)(
        IDispatchEx* This,
        BSTR bstrName,
        DWORD grfdex);

    HRESULT (STDMETHODCALLTYPE *DeleteMemberByDispID)(
        IDispatchEx* This,
        DISPID id);

    HRESULT (STDMETHODCALLTYPE *GetMemberProperties)(
        IDispatchEx* This,
        DISPID id,
        DWORD grfdexFetch,
        DWORD* pgrfdex);

    HRESULT (STDMETHODCALLTYPE *GetMemberName)(
        IDispatchEx* This,
        DISPID id,
        BSTR* pbstrName);

    HRESULT (STDMETHODCALLTYPE *GetNextDispID)(
        IDispatchEx* This,
        DWORD grfdex,
        DISPID id,
        DISPID* pid);

    HRESULT (STDMETHODCALLTYPE *GetNameSpaceParent)(
        IDispatchEx* This,
        IUnknown** ppunk);

    END_INTERFACE
};

#ifdef COBJMACROS
/*** IUnknown methods ***/
#define IDispatchEx_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDispatchEx_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDispatchEx_Release(p) (p)->lpVtbl->Release(p)
/*** IDispatch methods ***/
#define IDispatchEx_GetTypeInfoCount(p,a) (p)->lpVtbl->GetTypeInfoCount(p,a)
#define IDispatchEx_GetTypeInfo(p,a,b,c) (p)->lpVtbl->GetTypeInfo(p,a,b,c)
#define IDispatchEx_GetIDsOfNames(p,a,b,c,d,e) (p)->lpVtbl->GetIDsOfNames(p,a,b,c,d,e)
#define IDispatchEx_Invoke(p,a,b,c,d,e,f,g,h) (p)->lpVtbl->Invoke(p,a,b,c,d,e,f,g,h)
/*** IDispatchEx methods ***/
#define IDispatchEx_GetDispID(p,a,b,c) (p)->lpVtbl->GetDispID(p,a,b,c)
#define IDispatchEx_InvokeEx(p,a,b,c,d,e,f,g) (p)->lpVtbl->InvokeEx(p,a,b,c,d,e,f,g)
#define IDispatchEx_DeleteMemberByName(p,a,b) (p)->lpVtbl->DeleteMemberByName(p,a,b)
#define IDispatchEx_DeleteMemberByDispID(p,a) (p)->lpVtbl->DeleteMemberByDispID(p,a)
#define IDispatchEx_GetMemberProperties(p,a,b,c) (p)->lpVtbl->GetMemberProperties(p,a,b,c)
#define IDispatchEx_GetMemberName(p,a,b) (p)->lpVtbl->GetMemberName(p,a,b)
#define IDispatchEx_GetNextDispID(p,a,b,c) (p)->lpVtbl->GetNextDispID(p,a,b,c)
#define IDispatchEx_GetNameSpaceParent(p,a) (p)->lpVtbl->GetNameSpaceParent(p,a)
#endif

#endif

#define IDispatchEx_METHODS \
    ICOM_MSVTABLE_COMPAT_FIELDS \
    /*** IUnknown methods ***/ \
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE; \
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; \
    STDMETHOD_(ULONG,Release)(THIS) PURE; \
    /*** IDispatch methods ***/ \
    STDMETHOD_(HRESULT,GetTypeInfoCount)(THIS_ UINT* pctinfo) PURE; \
    STDMETHOD_(HRESULT,GetTypeInfo)(THIS_ UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) PURE; \
    STDMETHOD_(HRESULT,GetIDsOfNames)(THIS_ REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) PURE; \
    STDMETHOD_(HRESULT,Invoke)(THIS_ DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) PURE; \
    /*** IDispatchEx methods ***/ \
    STDMETHOD_(HRESULT,GetDispID)(THIS_ BSTR bstrName, DWORD grfdex, DISPID* pid) PURE; \
    STDMETHOD_(HRESULT,InvokeEx)(THIS_ DISPID id, LCID lcid, WORD wFlags, DISPPARAMS* pdp, VARIANT* pvarRes, EXCEPINFO* pei, IServiceProvider* pspCaller) PURE; \
    STDMETHOD_(HRESULT,DeleteMemberByName)(THIS_ BSTR bstrName, DWORD grfdex) PURE; \
    STDMETHOD_(HRESULT,DeleteMemberByDispID)(THIS_ DISPID id) PURE; \
    STDMETHOD_(HRESULT,GetMemberProperties)(THIS_ DISPID id, DWORD grfdexFetch, DWORD* pgrfdex) PURE; \
    STDMETHOD_(HRESULT,GetMemberName)(THIS_ DISPID id, BSTR* pbstrName) PURE; \
    STDMETHOD_(HRESULT,GetNextDispID)(THIS_ DWORD grfdex, DISPID id, DISPID* pid) PURE; \
    STDMETHOD_(HRESULT,GetNameSpaceParent)(THIS_ IUnknown** ppunk) PURE;

HRESULT CALLBACK IDispatchEx_GetDispID_Proxy(
    IDispatchEx* This,
    BSTR bstrName,
    DWORD grfdex,
    DISPID* pid);
void __RPC_STUB IDispatchEx_GetDispID_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_RemoteInvokeEx_Proxy(
    IDispatchEx* This,
    DISPID id,
    LCID lcid,
    DWORD dwFlags,
    DISPPARAMS* pdp,
    VARIANT* pvarRes,
    EXCEPINFO* pei,
    IServiceProvider* pspCaller,
    UINT cvarRefArg,
    UINT* rgiRefArg,
    VARIANT* rgvarRefArg);
void __RPC_STUB IDispatchEx_RemoteInvokeEx_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_InvokeEx_Proxy(
    IDispatchEx* This,
    DISPID id,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS* pdp,
    VARIANT* pvarRes,
    EXCEPINFO* pei,
    IServiceProvider* pspCaller);
HRESULT __RPC_STUB IDispatchEx_InvokeEx_Stub(
    IDispatchEx* This,
    DISPID id,
    LCID lcid,
    DWORD dwFlags,
    DISPPARAMS* pdp,
    VARIANT* pvarRes,
    EXCEPINFO* pei,
    IServiceProvider* pspCaller,
    UINT cvarRefArg,
    UINT* rgiRefArg,
    VARIANT* rgvarRefArg);
HRESULT CALLBACK IDispatchEx_DeleteMemberByName_Proxy(
    IDispatchEx* This,
    BSTR bstrName,
    DWORD grfdex);
void __RPC_STUB IDispatchEx_DeleteMemberByName_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_DeleteMemberByDispID_Proxy(
    IDispatchEx* This,
    DISPID id);
void __RPC_STUB IDispatchEx_DeleteMemberByDispID_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_GetMemberProperties_Proxy(
    IDispatchEx* This,
    DISPID id,
    DWORD grfdexFetch,
    DWORD* pgrfdex);
void __RPC_STUB IDispatchEx_GetMemberProperties_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_GetMemberName_Proxy(
    IDispatchEx* This,
    DISPID id,
    BSTR* pbstrName);
void __RPC_STUB IDispatchEx_GetMemberName_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_GetNextDispID_Proxy(
    IDispatchEx* This,
    DWORD grfdex,
    DISPID id,
    DISPID* pid);
void __RPC_STUB IDispatchEx_GetNextDispID_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispatchEx_GetNameSpaceParent_Proxy(
    IDispatchEx* This,
    IUnknown** ppunk);
void __RPC_STUB IDispatchEx_GetNameSpaceParent_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);

#endif  /* __IDispatchEx_INTERFACE_DEFINED__ */

#ifndef __IDispError_FWD_DEFINED__
#define __IDispError_FWD_DEFINED__
typedef struct IDispError IDispError;
#endif

/*****************************************************************************
 * IDispError interface
 */
#ifndef __IDispError_INTERFACE_DEFINED__
#define __IDispError_INTERFACE_DEFINED__

DEFINE_GUID(IID_IDispError, 0xa6ef9861, 0xc720, 0x11d0, 0x93,0x37, 0x00,0xa0,0xc9,0x0d,0xca,0xa9);
#if defined(__cplusplus) && !defined(CINTERFACE)
struct IDispError : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE QueryErrorInfo(
        GUID guidErrorType,
        IDispError** ppde) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNext(
        IDispError** ppde) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetHresult(
        HRESULT* phr) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetSource(
        BSTR* pbstrSource) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetHelpInfo(
        BSTR* pbstrFileName,
        DWORD* pdwContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDescription(
        BSTR* pbstrDescription) = 0;

};
#else
typedef struct IDispErrorVtbl IDispErrorVtbl;
struct IDispError {
    const IDispErrorVtbl* lpVtbl;
};
struct IDispErrorVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IDispError* This,
        REFIID riid,
        void** ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IDispError* This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IDispError* This);

    /*** IDispError methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryErrorInfo)(
        IDispError* This,
        GUID guidErrorType,
        IDispError** ppde);

    HRESULT (STDMETHODCALLTYPE *GetNext)(
        IDispError* This,
        IDispError** ppde);

    HRESULT (STDMETHODCALLTYPE *GetHresult)(
        IDispError* This,
        HRESULT* phr);

    HRESULT (STDMETHODCALLTYPE *GetSource)(
        IDispError* This,
        BSTR* pbstrSource);

    HRESULT (STDMETHODCALLTYPE *GetHelpInfo)(
        IDispError* This,
        BSTR* pbstrFileName,
        DWORD* pdwContext);

    HRESULT (STDMETHODCALLTYPE *GetDescription)(
        IDispError* This,
        BSTR* pbstrDescription);

    END_INTERFACE
};

#ifdef COBJMACROS
/*** IUnknown methods ***/
#define IDispError_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDispError_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDispError_Release(p) (p)->lpVtbl->Release(p)
/*** IDispError methods ***/
#define IDispError_QueryErrorInfo(p,a,b) (p)->lpVtbl->QueryErrorInfo(p,a,b)
#define IDispError_GetNext(p,a) (p)->lpVtbl->GetNext(p,a)
#define IDispError_GetHresult(p,a) (p)->lpVtbl->GetHresult(p,a)
#define IDispError_GetSource(p,a) (p)->lpVtbl->GetSource(p,a)
#define IDispError_GetHelpInfo(p,a,b) (p)->lpVtbl->GetHelpInfo(p,a,b)
#define IDispError_GetDescription(p,a) (p)->lpVtbl->GetDescription(p,a)
#endif

#endif

#define IDispError_METHODS \
    ICOM_MSVTABLE_COMPAT_FIELDS \
    /*** IUnknown methods ***/ \
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE; \
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; \
    STDMETHOD_(ULONG,Release)(THIS) PURE; \
    /*** IDispError methods ***/ \
    STDMETHOD_(HRESULT,QueryErrorInfo)(THIS_ GUID guidErrorType, IDispError** ppde) PURE; \
    STDMETHOD_(HRESULT,GetNext)(THIS_ IDispError** ppde) PURE; \
    STDMETHOD_(HRESULT,GetHresult)(THIS_ HRESULT* phr) PURE; \
    STDMETHOD_(HRESULT,GetSource)(THIS_ BSTR* pbstrSource) PURE; \
    STDMETHOD_(HRESULT,GetHelpInfo)(THIS_ BSTR* pbstrFileName, DWORD* pdwContext) PURE; \
    STDMETHOD_(HRESULT,GetDescription)(THIS_ BSTR* pbstrDescription) PURE;

HRESULT CALLBACK IDispError_QueryErrorInfo_Proxy(
    IDispError* This,
    GUID guidErrorType,
    IDispError** ppde);
void __RPC_STUB IDispError_QueryErrorInfo_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispError_GetNext_Proxy(
    IDispError* This,
    IDispError** ppde);
void __RPC_STUB IDispError_GetNext_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispError_GetHresult_Proxy(
    IDispError* This,
    HRESULT* phr);
void __RPC_STUB IDispError_GetHresult_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispError_GetSource_Proxy(
    IDispError* This,
    BSTR* pbstrSource);
void __RPC_STUB IDispError_GetSource_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispError_GetHelpInfo_Proxy(
    IDispError* This,
    BSTR* pbstrFileName,
    DWORD* pdwContext);
void __RPC_STUB IDispError_GetHelpInfo_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);
HRESULT CALLBACK IDispError_GetDescription_Proxy(
    IDispError* This,
    BSTR* pbstrDescription);
void __RPC_STUB IDispError_GetDescription_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);

#endif  /* __IDispError_INTERFACE_DEFINED__ */

#ifndef __IVariantChangeType_FWD_DEFINED__
#define __IVariantChangeType_FWD_DEFINED__
typedef struct IVariantChangeType IVariantChangeType;
#endif

/*****************************************************************************
 * IVariantChangeType interface
 */
#ifndef __IVariantChangeType_INTERFACE_DEFINED__
#define __IVariantChangeType_INTERFACE_DEFINED__

DEFINE_GUID(IID_IVariantChangeType, 0xa6ef9862, 0xc720, 0x11d0, 0x93,0x37, 0x00,0xa0,0xc9,0x0d,0xca,0xa9);
#if defined(__cplusplus) && !defined(CINTERFACE)
struct IVariantChangeType : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE ChangeType(
        VARIANT* pvarDst,
        VARIANT* pvarSrc,
        LCID lcid,
        VARTYPE vtNew) = 0;

};
#else
typedef struct IVariantChangeTypeVtbl IVariantChangeTypeVtbl;
struct IVariantChangeType {
    const IVariantChangeTypeVtbl* lpVtbl;
};
struct IVariantChangeTypeVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IVariantChangeType* This,
        REFIID riid,
        void** ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IVariantChangeType* This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IVariantChangeType* This);

    /*** IVariantChangeType methods ***/
    HRESULT (STDMETHODCALLTYPE *ChangeType)(
        IVariantChangeType* This,
        VARIANT* pvarDst,
        VARIANT* pvarSrc,
        LCID lcid,
        VARTYPE vtNew);

    END_INTERFACE
};

#ifdef COBJMACROS
/*** IUnknown methods ***/
#define IVariantChangeType_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IVariantChangeType_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IVariantChangeType_Release(p) (p)->lpVtbl->Release(p)
/*** IVariantChangeType methods ***/
#define IVariantChangeType_ChangeType(p,a,b,c,d) (p)->lpVtbl->ChangeType(p,a,b,c,d)
#endif

#endif

#define IVariantChangeType_METHODS \
    ICOM_MSVTABLE_COMPAT_FIELDS \
    /*** IUnknown methods ***/ \
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE; \
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; \
    STDMETHOD_(ULONG,Release)(THIS) PURE; \
    /*** IVariantChangeType methods ***/ \
    STDMETHOD_(HRESULT,ChangeType)(THIS_ VARIANT* pvarDst, VARIANT* pvarSrc, LCID lcid, VARTYPE vtNew) PURE;

HRESULT CALLBACK IVariantChangeType_ChangeType_Proxy(
    IVariantChangeType* This,
    VARIANT* pvarDst,
    VARIANT* pvarSrc,
    LCID lcid,
    VARTYPE vtNew);
void __RPC_STUB IVariantChangeType_ChangeType_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);

#endif  /* __IVariantChangeType_INTERFACE_DEFINED__ */

#ifndef __IObjectIdentity_FWD_DEFINED__
#define __IObjectIdentity_FWD_DEFINED__
typedef struct IObjectIdentity IObjectIdentity;
#endif

/*****************************************************************************
 * IObjectIdentity interface
 */
#ifndef __IObjectIdentity_INTERFACE_DEFINED__
#define __IObjectIdentity_INTERFACE_DEFINED__

DEFINE_GUID(IID_IObjectIdentity, 0xca04b7e6, 0x0d21, 0x11d1, 0x8c,0xc5, 0x00,0xc0,0x4f,0xc2,0xb0,0x85);
#if defined(__cplusplus) && !defined(CINTERFACE)
struct IObjectIdentity : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE IsEqualObject(
        IUnknown* punk) = 0;

};
#else
typedef struct IObjectIdentityVtbl IObjectIdentityVtbl;
struct IObjectIdentity {
    const IObjectIdentityVtbl* lpVtbl;
};
struct IObjectIdentityVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IObjectIdentity* This,
        REFIID riid,
        void** ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IObjectIdentity* This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IObjectIdentity* This);

    /*** IObjectIdentity methods ***/
    HRESULT (STDMETHODCALLTYPE *IsEqualObject)(
        IObjectIdentity* This,
        IUnknown* punk);

    END_INTERFACE
};

#ifdef COBJMACROS
/*** IUnknown methods ***/
#define IObjectIdentity_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IObjectIdentity_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IObjectIdentity_Release(p) (p)->lpVtbl->Release(p)
/*** IObjectIdentity methods ***/
#define IObjectIdentity_IsEqualObject(p,a) (p)->lpVtbl->IsEqualObject(p,a)
#endif

#endif

#define IObjectIdentity_METHODS \
    ICOM_MSVTABLE_COMPAT_FIELDS \
    /*** IUnknown methods ***/ \
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE; \
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; \
    STDMETHOD_(ULONG,Release)(THIS) PURE; \
    /*** IObjectIdentity methods ***/ \
    STDMETHOD_(HRESULT,IsEqualObject)(THIS_ IUnknown* punk) PURE;

HRESULT CALLBACK IObjectIdentity_IsEqualObject_Proxy(
    IObjectIdentity* This,
    IUnknown* punk);
void __RPC_STUB IObjectIdentity_IsEqualObject_Stub(
    struct IRpcStubBuffer* This,
    struct IRpcChannelBuffer* pRpcChannelBuffer,
    PRPC_MESSAGE pRpcMessage,
    DWORD* pdwStubPhase);

#endif  /* __IObjectIdentity_INTERFACE_DEFINED__ */

#ifdef __cplusplus
}
#endif
#endif /* __WIDL_DISPEX_H */
