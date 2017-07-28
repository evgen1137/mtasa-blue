/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.h
*  PURPOSE:     Resource manager class
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               Oliver Brown <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class controls all the resources that are loaded, and loads
// new resources on demand

#pragma once

#include "CResource.h"
#include "CElement.h"
#include "ehs/ehs.h"
#include <list>

class CResource;
#define INVALID_RESOURCE_NET_ID     0xFFFF

class CResourceManager 
{
public:
    enum eResourceQueue
    {
        QUEUE_STOP,
        QUEUE_STOPALL,
        QUEUE_RESTART,
        QUEUE_RESTART2,
        QUEUE_REFRESH,
        QUEUE_REFRESHALL,
    };

    struct sResourceStartFlags
    {
        bool bConfigs;
        bool bMaps;
        bool bScripts;
        bool bHTML;
        bool bClientConfigs;
        bool bClientScripts;
        bool bClientFiles;
    };

private:
    struct sResourceQueue
    {
        CResource*          pResource;
        eResourceQueue      eQueue;
        sResourceStartFlags Flags;
        vector < SString >  dependents;
    };

public:
    ZERO_ON_NEW
                                CResourceManager                ( void );
                                ~CResourceManager               ( void );

    CResource *                 Load                            ( bool bIsZipped, const char * szAbsPath, const char * szResourceName );
    void                        UnloadAndDelete                 ( CResource * resource );
    CResource *                 GetResource                     ( const char * szResourceName );
    CResource*                  GetResourceFromScriptID         ( uint uiScriptID );
    void                        UnloadRemovedResources          ( void );
    void                        CheckResourceDependencies       ( void );
    void                        ListResourcesLoaded             ( const SString& strListType );
    std::list < CResource* > ::const_iterator  IterBegin        ( void )            { return m_resources.begin (); };
    std::list < CResource* > ::const_iterator  IterEnd          ( void )            { return m_resources.end (); };

    bool                        Refresh                         ( bool bRefreshAll = false, const SString& strJustThisResource = "", bool bShowTiming = false );
    void                        UpgradeResources                ( CResource* pResource = NULL );
    void                        CheckResources                  ( CResource* pResource = NULL );
    inline unsigned int         GetResourceLoadedCount          ( void )            { return m_uiResourceLoadedCount; }
    inline unsigned int         GetResourceFailedCount          ( void )            { return m_uiResourceFailedCount; }
    void                        OnPlayerJoin                    ( CPlayer& Player );
    
    const char*                 GetResourceDirectory            ( void );

    bool                        StartResource                   ( CResource* pResource, list < CResource* > * dependents = NULL, bool bStartedManually = false, bool bStartIncludedResources = true, bool bConfigs = true, bool bMaps = true, bool bScripts = true, bool bHTML = true, bool bClientConfigs = true, bool bClientScripts = true, bool bClientFiles = true );
    bool                        Reload                          ( CResource* pResource );
    bool                        StopAllResources                ( void );
    uint64                      GetTotalResourcesSize           ( void );

    void                        QueueResource                   ( CResource* pResource, eResourceQueue eQueueType, const sResourceStartFlags* Flags, list < CResource* > * dependents = NULL );
    void                        ProcessQueue                    ( void );
    void                        RemoveFromQueue                 ( CResource* pResource );

    bool                        IsAResourceElement              ( CElement* pElement );

    unsigned short              GenerateID                      ( void );
    CResource*                  GetResourceFromNetID            ( unsigned short usNetID );

    CResource*                  GetResourceFromLuaState         ( struct lua_State* luaVM );
    SString                     GetResourceName                 ( struct lua_State* luaVM );

    CResource*                  CreateResource                  ( const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus );
    CResource*                  CopyResource                    ( CResource* pSourceResource, const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus );
    CResource*                  RenameResource                  ( CResource* pSourceResource, const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus );
    bool                        DeleteResource                  ( const SString& strResourceName, SString& strOutStatus );

    SString                     GetResourceTrashDir             ( void );
    bool                        MoveDirToTrash                  ( const SString& strPathDirName );
    SString                     GetResourceOrganizationalPath   ( CResource* pResource );

    static bool                 ParseResourcePathInput          ( std::string strInput, CResource*& pResource, std::string* pStrPath, std::string* pStrMetaPath = nullptr );

    void                        NotifyResourceVMOpen            ( CResource* pResource, CLuaMain* pVM );
    void                        NotifyResourceVMClose           ( CResource* pResource, CLuaMain* pVM );

    void                        AddResourceToLists              ( CResource* pResource );
    void                        RemoveResourceFromLists         ( CResource* pResource );

    void                        ApplyMinClientRequirement       ( CResource* pResource, const SString& strMinClientRequirement );
    void                        RemoveMinClientRequirement      ( CResource* pResource );
    void                        ReevaluateMinClientRequirement  ( void );
    SString                     GetMinClientRequirement         ( void )            { return m_strMinClientRequirement; }

    void                        ApplySyncMapElementDataOption       ( CResource* pResource, bool bSyncMapElementData );
    void                        RemoveSyncMapElementDataOption      ( CResource* pResource );
    void                        ReevaluateSyncMapElementDataOption  ( void );

    void                        LoadBlockedFileReasons          ( void );
    void                        SaveBlockedFileReasons          ( void );
    void                        ClearBlockedFileReason          ( const SString& strFileHash );
    void                        AddBlockedFileReason            ( const SString& strFileHash, const SString& strReason );
    SString                     GetBlockedFileReason            ( const SString& strFileHash );

private:
    SString                     m_strResourceDirectory;
    CMappedList < CResource* >  m_resources;
    unsigned int                m_uiResourceLoadedCount;
    unsigned int                m_uiResourceFailedCount;
    bool                        m_bResourceListChanged;
    list<CResource *>           m_resourcesToStartAfterRefresh;

    // Maps to speed things up
    CFastHashMap < CResource*, lua_State* > m_ResourceLuaStateMap;
    CFastHashMap < lua_State*, CResource* > m_LuaStateResourceMap;
    CFastHashMap < SString, CResource* >    m_NameResourceMap;
    std::map < ushort, CResource* >         m_NetIdResourceMap;

    list<sResourceQueue>        m_resourceQueue;

    SString                                 m_strMinClientRequirement;
    CFastHashMap < CResource*, SString >    m_MinClientRequirementMap;
    CFastHashMap < CResource*, bool >       m_SyncMapElementDataOptionMap;

    ushort                      m_usNextNetId;
    std::map < SString, SString > m_BlockedFileReasonMap;
};
