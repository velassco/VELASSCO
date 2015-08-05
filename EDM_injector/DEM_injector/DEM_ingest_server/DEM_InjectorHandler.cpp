// DEM_ingest_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"


using namespace  ::DEM_Inject;

#include "DEM_InjectorHandler.h"


#define newObject(className) new(m)className(m)



void gen_random(char *s, const int len) {
   static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
   }
   s[len] = 0;
}
/**
* returns a session if if the user exists with the specified password and the specified role or an empty role.
*
* @param user_name
* @param role
* @param password
*/
void DEM_InjectorHandler::UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password) {
   char sessionID[64];
   gen_random(sessionID, sizeof(sessionID)); _return = sessionID;
}

/**
* Stop access to the system by a given session id and release all resources held by that session
*
* @param sessionID
*/
void DEM_InjectorHandler::UserLogout(std::string& _return, const std::string& sessionID) {
   _return = "OK";
}

/**
* Stores a DEM_simulation with all timesteps, meshes, particles, results etc. in one operation in EDM
* If the model exists, the Simulation is added to the model. If the model does not exist, it is created.
*
* @param sessionID
* @param model_name
* @param theSimulation
*/
void DEM_InjectorHandler::StoreDEM_Simulation(std::string& _return, const std::string& sessionID, const std::string& model_name, const DEM_Inject::Simulation& theSimulation)
{
   try {
      setCurrentSession(sessionID.data());
      setCurrentModel(model_name.data());
      CMemoryAllocator *ma = m->getMemoryAllocator();
      m->reset();
      
      printf("StoreDEM_Simulation\n");
      dem::Simulation *sim = newObject(dem::Simulation);
      sim->put_name((char*)theSimulation.name.data());

      vector<DEM_Inject::Timestep> tsteps = theSimulation.consists_of;
      printf("There are %d Timesteps\nEach timestep has the following attributes:\n", tsteps.size());
      vector<DEM_Inject::Timestep>::iterator ts = tsteps.begin();
      for (ts; ts != tsteps.end(); ts++) {
         dem::Timestep *dts = newObject(dem::Timestep );
         sim->put_consists_of_element(dts); dts->put_time_value(ts->time_value);
         vector<DEM_Inject::Particle>::iterator pa = ts->consists_of.begin();
         for (pa; pa != ts->consists_of.end(); pa++) {
            dem::Particle *p;
            if (pa->type == Sphere_Particle) {
               dem::Sphere *sp = newObject(dem::Sphere); p = sp; sp->put_radius(pa->radius);
            } else {
               dem::Template_nn *tn = newObject(dem::Template_nn); p = tn;
            }
            p->put_id(pa->id); p->put_group(pa->group);
            if (particles[pa->id]) {
               _return = "particle already defined"; return;
            } else {
               particles[pa->id] = p;
            }
         }
         vector<DEM_Inject::Contact>::iterator contact = ts->has_contact.begin();
         for (contact; contact != ts->has_contact.end(); contact++) {
            dem::Contact *dcp;
            dem::Particle *p;
            if (contact->type == Geometry_Contact) {
               dem::Particle_Geometry_contact *dpgc = newObject(dem::Particle_Geometry_contact); dcp = dpgc;
            } else {
               dem::Particle_Particle_contact *dppc = newObject(dem::Particle_Particle_contact); dcp = dppc;
               if (contact->P1_id && (p = particles[contact->P1_id])) {
                  dppc->put_P1(p);
               }
               if (contact->P2_id && (p = particles[contact->P2_id])) {
                  dppc->put_P2(p);
               }
            }
            dts->put_has_contact_element(dcp);
            vector<double>::iterator coordIter = contact->contact_location.begin();
            for (int i = 1; i < 4; i++) {
               if (coordIter == contact->contact_location.end())
                  break;
               dcp->put_contact_location_element(i, *coordIter); coordIter++;
            }
         }
      }
      m->writeAllObjectsToDatabase();

      _return = "OK";
   } catch (CedmError *e) {
      if (e->message) {
         _return = e->message;
      } else {
         _return = edmiGetErrorText(e->rstat);
      }
      delete e;
   }
}

/**
* Stores a DEM_simulation with all timesteps, meshes, particles, results etc. in one operation in EDM
* If the model exists, the Simulation is added to the model. If the model does not exist, it is created.
*
* @param sessionID
* @param model_name
*/
void DEM_InjectorHandler::DeleteModelContent(std::string& _return, const std::string& sessionID, const std::string& model_name)
{
   try {
      setCurrentSession(sessionID.data());
      setCurrentModel(model_name.data());
      CHECK(edmiDeleteModelContents(m->modelId));
   } catch (CedmError *e) {
      if (e->message) {
         _return = e->message;
      } else {
         _return = edmiGetErrorText(e->rstat);
      }
      delete e;
   }
}



