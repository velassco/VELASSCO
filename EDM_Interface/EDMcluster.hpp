#ifndef EDMcluster_HPP
#define EDMcluster_HPP
#pragma warning( disable : 4091 )

extern tEdmiEntityData EDMcluster_Entities[];

namespace EDMcluster {

class EDMcluster_Schema;
class ClusterRepository;
class EDMServer;
class EDMcluster;
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
   void                                 put_consists_of_element(EDMrepository*);
   EDMcluster*                          get_platform();
   void                                 put_platform(EDMcluster* v);
   void                                 unset_platform() { unsetAttribute(1); }
   bool                                 exists_platform() { return isAttrSet(1); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(2); }
   bool                                 exists_name() { return isAttrSet(2); }
   Set<ClusterModel*>*                  get_models();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ClusterRepository(Model *m, entityType et=et_ClusterRepository) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ClusterRepository(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
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
   EDMcluster*                          get_cluster();
   void                                 put_cluster(EDMcluster* v);
   void                                 unset_cluster() { unsetAttribute(4); }
   bool                                 exists_cluster() { return isAttrSet(4); }
   int                                  get_nAppservers();
   void                                 put_nAppservers(int v);
   void                                 unset_nAppservers() { unsetAttribute(5); }
   bool                                 exists_nAppservers() { return isAttrSet(5); }
   EDMdatabase*                         get_runs();
   void                                 put_runs(EDMdatabase* v);
   void                                 unset_runs() { unsetAttribute(6); }
   bool                                 exists_runs() { return isAttrSet(6); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMServer(Model *m, entityType et=et_EDMServer) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMServer(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
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
   Set<EDMdatabase*>*                   get_databases();
   Set<EDMServer*>*                     get_servers();
   Set<ClusterRepository*>*             get_repositories();
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   EDMcluster(Model *m, entityType et=et_EDMcluster) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   EDMcluster(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class EDMdatabase :  public dbInstance
{
protected:
   EDMdatabase() {}
public:
   static const entityType type = et_EDMdatabase;
   EDMcluster*                          get_belongs_to();
   void                                 put_belongs_to(EDMcluster* v);
   void                                 unset_belongs_to() { unsetAttribute(0); }
   bool                                 exists_belongs_to() { return isAttrSet(0); }
   char *                               get_path();
   void                                 put_path(char * v);
   void                                 unset_path() { unsetAttribute(1); }
   bool                                 exists_path() { return isAttrSet(1); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(2); }
   bool                                 exists_name() { return isAttrSet(2); }
   char *                               get_password();
   void                                 put_password(char * v);
   void                                 unset_password() { unsetAttribute(3); }
   bool                                 exists_password() { return isAttrSet(3); }
   Set<EDMServer*>*                     get_server();
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
   void                                 put_consists_of_element(EDMmodel*);
   ClusterRepository*                   get_belongs_to();
   void                                 put_belongs_to(ClusterRepository* v);
   void                                 unset_belongs_to() { unsetAttribute(1); }
   bool                                 exists_belongs_to() { return isAttrSet(1); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(2); }
   bool                                 exists_name() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ClusterModel(Model *m, entityType et=et_ClusterModel) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ClusterModel(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};

class EDMcluster_Schema : public dbSchema
{
public:
                           EDMcluster_Schema(tEdmiEntityData* ep, tEdmiDefinedTypeData *dt, const char **dtn) : dbSchema(ep, dt, dtn) { }
   void                    *generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma);
};

typedef bool (*supertypeCastingFunc) (entityType wantedSuperType, void **p);

typedef dbInstance* (*dbInstanceCastingFunc) (void *p);

extern void *supertype_cast(entityType wantedSuperType, void *p, entityType subType);

extern dbInstance *dbInstance_cast(void *p, entityType subType);

extern const char *EDMcluster_DefinedTypeNames[];

extern EDMcluster_Schema EDMcluster_SchemaObject;
}
#endif
