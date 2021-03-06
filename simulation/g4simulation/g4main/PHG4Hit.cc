#include "PHG4Hit.h"

#include <TSystem.h> // for gSystem

#include <cassert>
#include <cstdlib>
#include <type_traits>

using namespace std;

void
PHG4Hit::CopyFrom(const PHObject *phobj)
{
  const PHG4Hit *g4hit = dynamic_cast<const PHG4Hit *> (phobj);
  assert(g4hit);
  for (int i =0; i<2; i++)
    {
      set_x(i,g4hit->get_x(i));
      set_y(i,g4hit->get_y(i));
      set_z(i,g4hit->get_z(i));
      set_t(i,g4hit->get_t(i));
    }
  set_hit_id(g4hit->get_hit_id());
  set_trkid(g4hit->get_trkid());
  set_edep(g4hit->get_edep());
  for (unsigned char ic = 0; ic < UCHAR_MAX; ic++)
    {
      PROPERTY prop_id = static_cast<PHG4Hit::PROPERTY> (ic);
      if (g4hit->has_property(prop_id))
        {
	  set_property_nocheck(prop_id,g4hit->get_property_nocheck(prop_id));
	}
    }
  set_hit_type(g4hit->get_hit_type());
}


void
PHG4Hit::identify(ostream& os) const
{
  cout << "Class " << this->ClassName() << endl;
  cout << "x0: " << get_x(0)
       << ", y0: " << get_y(0)
       << ", z0: " << get_z(0)
       << ", t0: " << get_t(0) << endl;
  cout << "x1: " << get_x(1)
       << ", y1: " << get_y(1)
       << ", z1: " << get_z(1)
       << ", t1: " << get_t(1) << endl;
  cout     << "trackid: " << get_trkid() << ", edep: " << get_edep() << endl;
  cout     << "strip_z_index: " << get_strip_z_index() << ", strip_y_index: " << get_strip_y_index() << endl;
  cout     << "ladder_z_index: " << get_ladder_z_index() << ", ladder_phi_index: " << get_ladder_phi_index() << endl;
  cout     << "stave_index: " << get_property_int(prop_stave_index) << " half_stave_index " << get_property_int(prop_half_stave_index) << endl;
  cout     << "module_index: " << get_property_int(prop_module_index) << " chip_index " << get_property_int(prop_chip_index) << endl;
  cout << "layer id: " << get_layer() << ", scint_id: " << get_scint_id() << endl;
  cout << "hit type: " << get_hit_type() << endl; 
  return;
}

ostream& operator<<(ostream& stream, const PHG4Hit * hit){
  stream <<endl<< "(x,y,z) = "<< "("<<hit->get_avg_x()<<", "<<hit->get_avg_y()<<", "<<hit->get_avg_z()<<")"<<endl;
	stream << "trackid: " << hit->get_trkid()<<" hitid: "<<hit->get_hit_id()<<" layer: "<<hit->get_layer()<<endl;
  return stream;
}

void
PHG4Hit::Reset()
{
  cout << "Reset not implemented by daughter class" << endl;
  return;
}

std::pair<const std::string,PHG4Hit::PROPERTY_TYPE> 
PHG4Hit::get_property_info(const PROPERTY prop_id) 
{
  switch (prop_id)
  {
  case  prop_eion:
    return make_pair("ionizing energy loss",PHG4Hit::type_float);
  case   prop_light_yield:
    return make_pair("light yield",PHG4Hit::type_float);
  case   prop_px_0:
    return make_pair("px in",PHG4Hit::type_float);
  case   prop_px_1:
    return make_pair("px out",PHG4Hit::type_float);
  case   prop_py_0:
    return make_pair("py in",PHG4Hit::type_float);
  case   prop_py_1:
    return make_pair("py out",PHG4Hit::type_float);
  case   prop_pz_0:
    return make_pair("pz in",PHG4Hit::type_float);
  case   prop_pz_1:
    return make_pair("pz out",PHG4Hit::type_float);
  case   prop_local_x_0:
    return make_pair("local x in",PHG4Hit::type_float);
  case   prop_local_x_1:
    return make_pair("local x out",PHG4Hit::type_float);
  case   prop_local_y_0:
    return make_pair("local y in",PHG4Hit::type_float);
  case   prop_local_y_1:
    return make_pair("local y out",PHG4Hit::type_float);
  case   prop_local_z_0:
    return make_pair("local z in",PHG4Hit::type_float);
  case   prop_local_z_1:
    return make_pair("local z out",PHG4Hit::type_float);
  case   prop_path_length:
    return make_pair("pathlength",PHG4Hit::type_float);
  case   prop_layer:
    return make_pair("layer ID",PHG4Hit::type_uint);
  case   prop_scint_id:
    return make_pair("scintillator ID",PHG4Hit::type_int);
  case   prop_row:
    return make_pair("row",PHG4Hit::type_int);
  case   prop_strip_z_index:
    return make_pair("strip z index",PHG4Hit::type_int);
  case   prop_strip_y_index:
    return make_pair("strip y index",PHG4Hit::type_int);
  case   prop_ladder_z_index:
    return make_pair("ladder z index",PHG4Hit::type_int);
  case   prop_ladder_phi_index:
    return make_pair("ladder phi index",PHG4Hit::type_int);
  case   prop_index_i:
    return make_pair("generic index i",PHG4Hit::type_int);
  case   prop_index_j:
    return make_pair("generic index j",PHG4Hit::type_int);
  case   prop_index_k:
    return make_pair("generic index k",PHG4Hit::type_int);
  case   prop_index_l:
    return make_pair("generic index l",PHG4Hit::type_int);
  case  prop_stave_index:
    return make_pair("stave index",PHG4Hit::type_int);
  case  prop_half_stave_index:
    return make_pair("half stave index",PHG4Hit::type_int);
  case  prop_module_index:
    return make_pair("module index",PHG4Hit::type_int);
  case  prop_chip_index:
    return make_pair("chip index",PHG4Hit::type_int);
  case  prop_local_pos_x_0:
    return make_pair("local x pos in",PHG4Hit::type_float);
  case  prop_local_pos_y_0:
    return make_pair("local y pos in",PHG4Hit::type_float);
  case  prop_local_pos_z_0:
    return make_pair("local z pos in",PHG4Hit::type_float);
  case   prop_hit_type:
    return make_pair("hit type",PHG4Hit::type_int);    
  case  prop_local_pos_x_1:
    return make_pair("local x pos out",PHG4Hit::type_float);
  case  prop_local_pos_y_1:
    return make_pair("local y pos out",PHG4Hit::type_float);
  case  prop_local_pos_z_1:
    return make_pair("local z pos out",PHG4Hit::type_float);

  default:
    cout << "PHG4Hit::get_property_info - Fatal Error - unknown index " << prop_id << endl;
    gSystem->Exit(1);
    exit(1);
  }
}


bool
PHG4Hit::check_property(const PROPERTY prop_id, const PROPERTY_TYPE prop_type)
{
  pair<const string,PROPERTY_TYPE> property_info = get_property_info(prop_id);
  if (property_info.second != prop_type)
    {
      return false;
    }
  return true;
}

string
PHG4Hit::get_property_type(const PROPERTY_TYPE prop_type)
{
  switch(prop_type)
    {
    case type_int:
      return "int";
    case type_uint:
      return "unsigned int";
    case type_float:
      return "float";
    default:
      return "unkown";
    }
}
