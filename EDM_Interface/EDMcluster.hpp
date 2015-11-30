#ifndef EDMcluster_HPP
#define EDMcluster_HPP
#pragma warning( disable : 4091 )

extern tEdmiEntityData EDMcluster_Entities[];

namespace ecl {

class ecl_Schema;
class ClusterRepository;
class EDMcluster;
class EDMServer;
class EDMdatabase;
class EDMrepository;
class EDMmodel;
class ClusterModel;
typedef enum {} definedTypeNames;


class ClusterRepository :  public dbInstance
{
protected:
   ClusterRepository() {}
public:
   static const entityType type = et_ClusterRepository;
   Set<EDMrepository*>*                 get_consists_of();
   void                                 put_consists_of(Set<EDMrepository*>* v);
   void                                 unset_consists_of() { unsetAttribute(0); }
   bool                                 exists_consists_of() { return isAttrSet(0); }
   SdaiAggr                             get_consists_of_aggrId();
   void                                 put_consists_of_element(EDMrepository*);
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   Set<ClusterModel*>*                  get_models();
   void                                 put_models(Set<ClusterModel*>* v);
   void                                 unset_models() { unsetAttribute(2); }
   bool                                 exists_models() { return isAttrSet(2); }
   SdaiAggr                             get_models_aggrId();
   void                                 put_models_element(ClusterModel*);
   Set<EDMcluster*>*                    get_platform();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ClusterRepository(Model *m, entityType et=et_ClusterRepository) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ClusterRepository(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class EDMcluster :  public dbInstance
{
protected:
   EDMcluster() {}
public:
   static const entityType type = et_EDMcluster;
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(0); }
   bool                                 exists_name() { return isAttrSet(0); }
   char *                               get_description();
   void                                 put_description(char * v);
   void                                 unset_description() { unsetAttribute(1); }
   bool                                 exists_description() { return isAttrSet(1); }
   Set<EDMServer*>*                     get_servers();
   void                                 put_servers(Set<EDMServer*>* v);
   void                                 unset_servers() { unsetAttribute(2); }
   bool                                 exists_servers() { return isAttrSet(2); }
   SdaiAggr                             get_servers_aggrId();
   void                                 put_servers_element(EDMServer*);
   Set<ClusterRepository*>*             get_repositories();
   void                                 put_repositories(Set<ClusterRepository*>* v);
   void                                 unset_repositories() { unsetAttribute(3); }
   bool                                 exists_repositories() { return isAttrSet(3); }
   SdaiAggr                             get_repositories_aggrId();
   void                                 put_repositories_element(ClusterRepository*);
   Set<EDMdatabase*>*                   get_databases();
   void                                 put_databases(Set<EDMdatabase*>* v);
   void                                 unset_databases() { unsetAttribute(4); }
   bool                                 exists_databases() { return isAttrSet(4); }
   SdaiAggr                             get_databases_aggrId();
   void                                 put_databases_element(EDMdatabase*);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMcluster(Model *m, entityType et=et_EDMcluster) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMcluster(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class EDMServer :  public dbInstance
{
protected:
   EDMServer() {}
public:
   static const entityType type = et_EDMServer;
   char *                               get_Name();
   void                                 put_Name(char * v);
   void                                 unset_Name() { unsetAttribute(0); }
   bool                                 exists_Name() { return isAttrSet(0); }
   char *                               get_Description();
   void                                 put_Description(char * v);
   void                                 unset_Description() { unsetAttribute(1); }
   bool                                 exists_Description() { return isAttrSet(1); }
   char *                               get_Host();
   void                                 put_Host(char * v);
   void                                 unset_Host() { unsetAttribute(2); }
   bool                                 exists_Host() { return isAttrSet(2); }
   char *                               get_Port();
   void                                 put_Port(char * v);
   void                                 unset_Port() { unsetAttribute(3); }
   bool                                 exists_Port() { return isAttrSet(3); }
   int                                  get_nAppservers();
   void                                 put_nAppservers(int v);
   void                                 unset_nAppservers() { unsetAttribute(4); }
   bool                                 exists_nAppservers() { return isAttrSet(4); }
   Set<EDMcluster*>*                    get_cluster();
   Set<EDMdatabase*>*                   get_runs();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMServer(Model *m, entityType et=et_EDMServer) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMServer(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class EDMdatabase :  public dbInstance
{
protected:
   EDMdatabase() {}
public:
   static const entityType type = et_EDMdatabase;
   char *                               get_path();
   void                                 put_path(char * v);
   void                                 unset_path() { unsetAttribute(0); }
   bool                                 exists_path() { return isAttrSet(0); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   char *                               get_password();
   void                                 put_password(char * v);
   void                                 unset_password() { unsetAttribute(2); }
   bool                                 exists_password() { return isAttrSet(2); }
   EDMServer*                           get_server();
   void                                 put_server(EDMServer* v);
   void                                 unset_server() { unsetAttribute(3); }
   bool                                 exists_server() { return isAttrSet(3); }
   Set<EDMcluster*>*                    get_belongs_to();
   Set<EDMrepository*>*                 get_repositories();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMdatabase(Model *m, entityType et=et_EDMdatabase) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMdatabase(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class EDMrepository :  public dbInstance
{
protected:
   EDMrepository() {}
public:
   static const entityType type = et_EDMrepository;
   EDMdatabase*                         get_belongs_to();
   void                                 put_belongs_to(EDMdatabase* v);
   void                                 unset_belongs_to() { unsetAttribute(0); }
   bool                                 exists_belongs_to() { return isAttrSet(0); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   Set<EDMmodel*>*                      get_models();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMrepository(Model *m, entityType et=et_EDMrepository) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMrepository(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class EDMmodel :  public dbInstance
{
protected:
   EDMmodel() {}
public:
   static const entityType type = et_EDMmodel;
   EDMrepository*                       get_repository();
   void                                 put_repository(EDMrepository* v);
   void                                 unset_repository() { unsetAttribute(0); }
   bool                                 exists_repository() { return isAttrSet(0); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMmodel(Model *m, entityType et=et_EDMmodel) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMmodel(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class ClusterModel :  public dbInstance
{
protected:
   ClusterModel() {}
public:
   static const entityType type = et_ClusterModel;
   Set<EDMmodel*>*                      get_consists_of();
   void                                 put_consists_of(Set<EDMmodel*>* v);
   void                                 unset_consists_of() { unsetAttribute(0); }
   bool                                 exists_consists_of() { return isAttrSet(0); }
   SdaiAggr                             get_consists_of_aggrId();
   void                                 put_consists_of_element(EDMmodel*);
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   Set<ClusterRepository*>*             get_belongs_to();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ClusterModel(Model *m, entityType et=et_ClusterModel) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ClusterModel(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};

class ecl_Schema : public dbSchema
{
public:
                           ecl_Schema(tEdmiEntityData* ep, tEdmiDefinedTypeData *dt, const char **dtn) : dbSchema(ep, dt, dtn) { }
   void                    *generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma);
};

typedef bool (*supertypeCastingFunc) (entityType wantedSuperType, void **p);

typedef dbInstance* (*dbInstanceCastingFunc) (void *p);

extern void *supertype_cast(entityType wantedSuperType, void *p, entityType subType);

extern dbInstance *dbInstance_cast(void *p, entityType subType);

extern const char *EDMcluster_DefinedTypeNames[];

extern ecl_Schema EDMcluster_SchemaObject;
}
#endif
