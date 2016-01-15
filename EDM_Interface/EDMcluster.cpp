#include "stdafx.h"
#include "EDMcluster_header.h"

namespace ecl {

EDMLONG dbInstance_AttributeLayout[] = {0};
EDMLONG EDMuser_AttributeLayout[] = {0};
tEdmiAttribute EDMuser_Attributes[] = {
   {NULL, 0, 0},
};
EDMLONG EDMuser_Subtypes[] = {0};
EDMLONG ClusterUser_AttributeLayout[] = {0};
tEdmiAttribute ClusterUser_Attributes[] = {
   {NULL, 0, 0},
};
EDMLONG ClusterUser_Subtypes[] = {0};
EDMLONG EDMmodel_AttributeLayout[] = {8, 4, 8, 0};
tEdmiAttribute EDMmodel_Attributes[] = {
   {"repository", 8, 0},
   {"name", 4, 0},
   {"clusterModel", 8, 0},
   {NULL, 0, 0},
};
EDMLONG EDMmodel_Subtypes[] = {0};
EDMLONG EDMrepository_AttributeLayout[] = {8, 4, 8, 9, 0};
tEdmiAttribute EDMrepository_Attributes[] = {
   {"belongs_to", 8, 0},
   {"name", 4, 0},
   {"clusterRepository", 8, 0},
   {"models", 9, 0},
   {NULL, 0, 0},
};
EDMLONG EDMrepository_Subtypes[] = {0};
EDMLONG EDMdatabase_AttributeLayout[] = {4, 4, 4, 8, 9, 9, 0};
tEdmiAttribute EDMdatabase_Attributes[] = {
   {"path", 4, 0},
   {"name", 4, 0},
   {"password", 4, 0},
   {"server", 8, 0},
   {"belongs_to", 9, 0},
   {"repositories", 9, 0},
   {NULL, 0, 0},
};
EDMLONG EDMdatabase_Subtypes[] = {0};
EDMLONG EDMServer_AttributeLayout[] = {4, 4, 4, 4, 0, 9, 9, 0};
tEdmiAttribute EDMServer_Attributes[] = {
   {"Name", 4, 0},
   {"Description", 4, 0},
   {"Host", 4, 0},
   {"Port", 4, 0},
   {"nAppservers", 0, 0},
   {"cluster", 9, 0},
   {"runs", 9, 0},
   {NULL, 0, 0},
};
EDMLONG EDMServer_Subtypes[] = {0};
EDMLONG EDMcluster_AttributeLayout[] = {4, 4, 9, 9, 9, 0};
tEdmiAttribute EDMcluster_Attributes[] = {
   {"name", 4, 0},
   {"description", 4, 0},
   {"servers", 9, 0},
   {"databases", 9, 0},
   {"repositories", 9, 0},
   {NULL, 0, 0},
};
EDMLONG EDMcluster_Subtypes[] = {0};
EDMLONG ClusterRepository_AttributeLayout[] = {4, 8, 9, 9, 0};
tEdmiAttribute ClusterRepository_Attributes[] = {
   {"name", 4, 0},
   {"platform", 8, 0},
   {"models", 9, 0},
   {"consists_of", 9, 0},
   {NULL, 0, 0},
};
EDMLONG ClusterRepository_Subtypes[] = {0};
EDMLONG ClusterModel_AttributeLayout[] = {4, 8, 9, 0};
tEdmiAttribute ClusterModel_Attributes[] = {
   {"name", 4, 0},
   {"belongs_to", 8, 0},
   {"consists_of", 9, 0},
   {NULL, 0, 0},
};
EDMLONG ClusterModel_Subtypes[] = {0};
const char *EDMcluster_DefinedTypeNames[] = {
NULL
};
tEdmiEntityData EDMcluster_Entities[] = {
{"indeterminate"},
{"EDMuser", 0, 647, 0, 0, 8, et_EDMuser, EDMuser_AttributeLayout, EDMuser_Subtypes, NULL, EDMuser_Attributes},
{"ClusterUser", 0, 645, 0, 0, 7, et_ClusterUser, ClusterUser_AttributeLayout, ClusterUser_Subtypes, NULL, ClusterUser_Attributes},
{"EDMmodel", 3, 633, 8, 40, 1, et_EDMmodel, EDMmodel_AttributeLayout, EDMmodel_Subtypes, NULL, EDMmodel_Attributes},
{"EDMrepository", 4, 635, 8, 48, 2, et_EDMrepository, EDMrepository_AttributeLayout, EDMrepository_Subtypes, NULL, EDMrepository_Attributes},
{"EDMdatabase", 6, 637, 8, 64, 3, et_EDMdatabase, EDMdatabase_AttributeLayout, EDMdatabase_Subtypes, NULL, EDMdatabase_Attributes},
{"EDMServer", 7, 639, 8, 72, 4, et_EDMServer, EDMServer_AttributeLayout, EDMServer_Subtypes, NULL, EDMServer_Attributes},
{"EDMcluster", 5, 641, 8, 56, 5, et_EDMcluster, EDMcluster_AttributeLayout, EDMcluster_Subtypes, NULL, EDMcluster_Attributes},
{"ClusterRepository", 4, 643, 8, 48, 6, et_ClusterRepository, ClusterRepository_AttributeLayout, ClusterRepository_Subtypes, NULL, ClusterRepository_Attributes},
{"ClusterModel", 3, 631, 8, 40, 0, et_ClusterModel, ClusterModel_AttributeLayout, ClusterModel_Subtypes, NULL, ClusterModel_Attributes},
{NULL},
};

/*====================================================================================================
   EDMuser
====================================================================================================*/
/*====================================================================================================
   ClusterUser
====================================================================================================*/
/*====================================================================================================
   EDMmodel
====================================================================================================*/
EDMrepository* EDMmodel::get_repository() { return getInstance(0, EDMrepository*, 0); }
void EDMmodel::put_repository(EDMrepository* v) { putInstance(0, EDMrepository*, v, repository, 0, 8); v->addToUsedIn(c); }
char * EDMmodel::get_name() { return getATTRIBUTE(8, char *, 1); }
void EDMmodel::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
ClusterModel* EDMmodel::get_clusterModel() { return getInstance(16, ClusterModel*, 2); }
void EDMmodel::put_clusterModel(ClusterModel* v) { putInstance(16, ClusterModel*, v, clusterModel, 2, 8); v->addToUsedIn(c); }
/*====================================================================================================
   EDMrepository
====================================================================================================*/
EDMdatabase* EDMrepository::get_belongs_to() { return getInstance(0, EDMdatabase*, 0); }
void EDMrepository::put_belongs_to(EDMdatabase* v) { putInstance(0, EDMdatabase*, v, belongs_to, 0, 8); v->addToUsedIn(c); }
char * EDMrepository::get_name() { return getATTRIBUTE(8, char *, 1); }
void EDMrepository::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
ClusterRepository* EDMrepository::get_clusterRepository() { return getInstance(16, ClusterRepository*, 2); }
void EDMrepository::put_clusterRepository(ClusterRepository* v) { putInstance(16, ClusterRepository*, v, clusterRepository, 2, 8); v->addToUsedIn(c); }
Set<EDMmodel*>* EDMrepository::get_models() { return getAGGREGATE(24, Set<EDMmodel*>*, 3); }
/*====================================================================================================
   EDMdatabase
====================================================================================================*/
char * EDMdatabase::get_path() { return getATTRIBUTE(0, char *, 0); }
void EDMdatabase::put_path(char * v) { putATTRIBUTE(0, char *, v, path, 0, 4); }
char * EDMdatabase::get_name() { return getATTRIBUTE(8, char *, 1); }
void EDMdatabase::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
char * EDMdatabase::get_password() { return getATTRIBUTE(16, char *, 2); }
void EDMdatabase::put_password(char * v) { putATTRIBUTE(16, char *, v, password, 2, 4); }
EDMServer* EDMdatabase::get_server() { return getInstance(24, EDMServer*, 3); }
void EDMdatabase::put_server(EDMServer* v) { putInstance(24, EDMServer*, v, server, 3, 8); v->addToUsedIn(c); }
Set<EDMcluster*>* EDMdatabase::get_belongs_to() { return getAGGREGATE(32, Set<EDMcluster*>*, 4); }
Set<EDMrepository*>* EDMdatabase::get_repositories() { return getAGGREGATE(40, Set<EDMrepository*>*, 5); }
/*====================================================================================================
   EDMServer
====================================================================================================*/
char * EDMServer::get_Name() { return getATTRIBUTE(0, char *, 0); }
void EDMServer::put_Name(char * v) { putATTRIBUTE(0, char *, v, Name, 0, 4); }
char * EDMServer::get_Description() { return getATTRIBUTE(8, char *, 1); }
void EDMServer::put_Description(char * v) { putATTRIBUTE(8, char *, v, Description, 1, 4); }
char * EDMServer::get_Host() { return getATTRIBUTE(16, char *, 2); }
void EDMServer::put_Host(char * v) { putATTRIBUTE(16, char *, v, Host, 2, 4); }
char * EDMServer::get_Port() { return getATTRIBUTE(24, char *, 3); }
void EDMServer::put_Port(char * v) { putATTRIBUTE(24, char *, v, Port, 3, 4); }
int EDMServer::get_nAppservers() { return getATTRIBUTE(32, int, 4); }
void EDMServer::put_nAppservers(int v) { putATTRIBUTE(32, int, v, nAppservers, 4, 0); }
Set<EDMcluster*>* EDMServer::get_cluster() { return getAGGREGATE(40, Set<EDMcluster*>*, 5); }
Set<EDMdatabase*>* EDMServer::get_runs() { return getAGGREGATE(48, Set<EDMdatabase*>*, 6); }
/*====================================================================================================
   EDMcluster
====================================================================================================*/
char * EDMcluster::get_name() { return getATTRIBUTE(0, char *, 0); }
void EDMcluster::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
char * EDMcluster::get_description() { return getATTRIBUTE(8, char *, 1); }
void EDMcluster::put_description(char * v) { putATTRIBUTE(8, char *, v, description, 1, 4); }
Set<EDMServer*>* EDMcluster::get_servers() { return getAGGREGATE(16, Set<EDMServer*>*, 2); }
void EDMcluster::put_servers(Set<EDMServer*>* v) { putAGGREGATE(16, Set<EDMServer*>*, v, servers, 2, 9); }
SdaiAggr  EDMcluster::get_servers_aggrId() { return getAGGRID(2); }
void EDMcluster::put_servers_element(EDMServer* element) {
   SdaiAggr agId = get_servers_aggrId();
   Set<EDMServer*> aggregate(m, agId);
   if (agId == 0) {
      put_servers(&aggregate);
   }
   aggregate.add(element);
}
Set<EDMdatabase*>* EDMcluster::get_databases() { return getAGGREGATE(24, Set<EDMdatabase*>*, 3); }
void EDMcluster::put_databases(Set<EDMdatabase*>* v) { putAGGREGATE(24, Set<EDMdatabase*>*, v, databases, 3, 9); }
SdaiAggr  EDMcluster::get_databases_aggrId() { return getAGGRID(3); }
void EDMcluster::put_databases_element(EDMdatabase* element) {
   SdaiAggr agId = get_databases_aggrId();
   Set<EDMdatabase*> aggregate(m, agId);
   if (agId == 0) {
      put_databases(&aggregate);
   }
   aggregate.add(element);
}
Set<ClusterRepository*>* EDMcluster::get_repositories() { return getAGGREGATE(32, Set<ClusterRepository*>*, 4); }
/*====================================================================================================
   ClusterRepository
====================================================================================================*/
char * ClusterRepository::get_name() { return getATTRIBUTE(0, char *, 0); }
void ClusterRepository::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
EDMcluster* ClusterRepository::get_platform() { return getInstance(8, EDMcluster*, 1); }
void ClusterRepository::put_platform(EDMcluster* v) { putInstance(8, EDMcluster*, v, platform, 1, 8); v->addToUsedIn(c); }
Set<ClusterModel*>* ClusterRepository::get_models() { return getAGGREGATE(16, Set<ClusterModel*>*, 2); }
Set<EDMrepository*>* ClusterRepository::get_consists_of() { return getAGGREGATE(24, Set<EDMrepository*>*, 3); }
/*====================================================================================================
   ClusterModel
====================================================================================================*/
char * ClusterModel::get_name() { return getATTRIBUTE(0, char *, 0); }
void ClusterModel::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
ClusterRepository* ClusterModel::get_belongs_to() { return getInstance(8, ClusterRepository*, 1); }
void ClusterModel::put_belongs_to(ClusterRepository* v) { putInstance(8, ClusterRepository*, v, belongs_to, 1, 8); v->addToUsedIn(c); }
Set<EDMmodel*>* ClusterModel::get_consists_of() { return getAGGREGATE(16, Set<EDMmodel*>*, 2); }

void* ecl_Schema::generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma)
{
   void* theObject = NULL;

   EDMuser* p_EDMuser;
   ClusterUser* p_ClusterUser;
   EDMmodel* p_EDMmodel;
   EDMrepository* p_EDMrepository;
   EDMdatabase* p_EDMdatabase;
   EDMServer* p_EDMServer;
   EDMcluster* p_EDMcluster;
   ClusterRepository* p_ClusterRepository;
   ClusterModel* p_ClusterModel;
   EDMLONG entityType = getEntityType((EDMLONG)instData->entityData);
   *entityTypep = entityType;
   instData->entityData = &theEntities[entityType];
   switch(entityType) {
         case et_EDMuser:
            p_EDMuser = new(ma) EDMuser(ma, instData);
            theObject = (void*)p_EDMuser;
            break;
         case et_ClusterUser:
            p_ClusterUser = new(ma) ClusterUser(ma, instData);
            theObject = (void*)p_ClusterUser;
            break;
         case et_EDMmodel:
            p_EDMmodel = new(ma) EDMmodel(ma, instData);
            theObject = (void*)p_EDMmodel;
            break;
         case et_EDMrepository:
            p_EDMrepository = new(ma) EDMrepository(ma, instData);
            theObject = (void*)p_EDMrepository;
            break;
         case et_EDMdatabase:
            p_EDMdatabase = new(ma) EDMdatabase(ma, instData);
            theObject = (void*)p_EDMdatabase;
            break;
         case et_EDMServer:
            p_EDMServer = new(ma) EDMServer(ma, instData);
            theObject = (void*)p_EDMServer;
            break;
         case et_EDMcluster:
            p_EDMcluster = new(ma) EDMcluster(ma, instData);
            theObject = (void*)p_EDMcluster;
            break;
         case et_ClusterRepository:
            p_ClusterRepository = new(ma) ClusterRepository(ma, instData);
            theObject = (void*)p_ClusterRepository;
            break;
         case et_ClusterModel:
            p_ClusterModel = new(ma) ClusterModel(ma, instData);
            theObject = (void*)p_ClusterModel;
            break;
default:
      theObject = NULL;
      break;
   }
   return theObject;
}
tEdmiDefinedTypeData EDMcluster_definedTypes[] = {
{NULL}
};

ecl_Schema EDMcluster_SchemaObject(EDMcluster_Entities, EDMcluster_definedTypes, EDMcluster_DefinedTypeNames);

static bool supertypeOf_EDMuser(entityType wantedSuperType, void **p);
static bool supertypeOf_ClusterUser(entityType wantedSuperType, void **p);
static bool supertypeOf_EDMmodel(entityType wantedSuperType, void **p);
static bool supertypeOf_EDMrepository(entityType wantedSuperType, void **p);
static bool supertypeOf_EDMdatabase(entityType wantedSuperType, void **p);
static bool supertypeOf_EDMServer(entityType wantedSuperType, void **p);
static bool supertypeOf_EDMcluster(entityType wantedSuperType, void **p);
static bool supertypeOf_ClusterRepository(entityType wantedSuperType, void **p);
static bool supertypeOf_ClusterModel(entityType wantedSuperType, void **p);
static dbInstance *dbInstanceOf_EDMuser(void *obj) { EDMuser *p = (EDMuser*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_EDMuser(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_EDMuser;
}

static dbInstance *dbInstanceOf_ClusterUser(void *obj) { ClusterUser *p = (ClusterUser*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ClusterUser(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ClusterUser;
}

static dbInstance *dbInstanceOf_EDMmodel(void *obj) { EDMmodel *p = (EDMmodel*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_EDMmodel(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_EDMmodel;
}

static dbInstance *dbInstanceOf_EDMrepository(void *obj) { EDMrepository *p = (EDMrepository*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_EDMrepository(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_EDMrepository;
}

static dbInstance *dbInstanceOf_EDMdatabase(void *obj) { EDMdatabase *p = (EDMdatabase*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_EDMdatabase(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_EDMdatabase;
}

static dbInstance *dbInstanceOf_EDMServer(void *obj) { EDMServer *p = (EDMServer*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_EDMServer(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_EDMServer;
}

static dbInstance *dbInstanceOf_EDMcluster(void *obj) { EDMcluster *p = (EDMcluster*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_EDMcluster(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_EDMcluster;
}

static dbInstance *dbInstanceOf_ClusterRepository(void *obj) { ClusterRepository *p = (ClusterRepository*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ClusterRepository(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ClusterRepository;
}

static dbInstance *dbInstanceOf_ClusterModel(void *obj) { ClusterModel *p = (ClusterModel*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ClusterModel(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ClusterModel;
}

static bool supertypeOf_indeterminate(entityType, void **)
{
   return false;
}


static supertypeCastingFunc castingFunctions[] = {
&supertypeOf_indeterminate,
   &supertypeOf_EDMuser,
   &supertypeOf_ClusterUser,
   &supertypeOf_EDMmodel,
   &supertypeOf_EDMrepository,
   &supertypeOf_EDMdatabase,
   &supertypeOf_EDMServer,
   &supertypeOf_EDMcluster,
   &supertypeOf_ClusterRepository,
   &supertypeOf_ClusterModel,
};
void *supertype_cast(entityType wantedSuperType, void *p, entityType subType)
{
   if (subType >= 0 && subType < 10) {
      void *superTypeObject = p;
      if (castingFunctions[subType](wantedSuperType, &superTypeObject)) {
         return superTypeObject;
      }
   }
   return NULL;
}
static dbInstance *dbInstanceOf_indeterminate(void *)
{
   return NULL;
}


static dbInstanceCastingFunc dbInstanceCastingFunctions[] = {
&dbInstanceOf_indeterminate,
   &dbInstanceOf_EDMuser,
   &dbInstanceOf_ClusterUser,
   &dbInstanceOf_EDMmodel,
   &dbInstanceOf_EDMrepository,
   &dbInstanceOf_EDMdatabase,
   &dbInstanceOf_EDMServer,
   &dbInstanceOf_EDMcluster,
   &dbInstanceOf_ClusterRepository,
   &dbInstanceOf_ClusterModel,
};
dbInstance *dbInstance_cast(void *p, entityType subType)
{
   if (subType < 0 || subType >= 9) {
      THROW("Illegal object type index");
   }
   return dbInstanceCastingFunctions[subType](p);
}


}
