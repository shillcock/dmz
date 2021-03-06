#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderObjectDataOSG.h>
#include "dmzRenderPluginLinkOSG.h"
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>

#include <osg/CullFace>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>

dmz::RenderPluginLinkOSG::RenderPluginLinkOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttrHandle (0),
      _hideAttrHandle (0),
      _render (0),
      _masterMask (0),
      _glyphMask (0),
      _entityMask (0),
      _cullMask (0) {

   _init (local);
}


dmz::RenderPluginLinkOSG::~RenderPluginLinkOSG () {

   _centerTable.empty ();
   _defTable.empty ();
   _linkTable.empty ();
   _objTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginLinkOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginLinkOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_render) {

         _render = RenderModuleCoreOSG::cast (PluginPtr);

         if (_render) {

            _masterMask = _render->get_master_isect_mask ();
            _glyphMask = _render->lookup_isect_mask (RenderIsectGlyphName);
            _entityMask = _render->lookup_isect_mask (RenderIsectEntityName);
            _cullMask = _render->get_cull_mask ();

            HashTableHandleIterator it;
            LinkDefStruct *def (0);

            while (_defTable.get_next (it, def)) { _create_geometry (*def); }

            it.reset ();
            LinkStruct *link (0);

            while (_linkTable.get_next (it, link)) { _create_link (*link); }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_render && (_render == RenderModuleCoreOSG::cast (PluginPtr))) {

         _masterMask = 0;
         _glyphMask = 0;
         _entityMask = 0;
         _cullMask = 0;
         _render = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginLinkOSG::update_time_slice (const Float64 TimeDelta) {

}


// Message Observer Interface
void
dmz::RenderPluginLinkOSG::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Object Observer Interface
void
dmz::RenderPluginLinkOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objTable.remove (ObjectHandle);

   if (os) { delete os; os = 0; }
}


void
dmz::RenderPluginLinkOSG::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LinkDefStruct *lds = _defTable.lookup (AttributeHandle);

   if (lds) {

      ObjectStruct *super = _lookup_object (SuperHandle);
      ObjectStruct *sub = _lookup_object (SubHandle);

      if (super && !super->superTable.lookup (LinkHandle) &&
            sub && !sub->subTable.lookup (LinkHandle)) {

         LinkStruct *ls = new LinkStruct (LinkHandle, *lds, *super, *sub);

         if (ls && _linkTable.store (LinkHandle, ls)) {

            _create_link (*ls);
         }
         else if (ls) { delete ls; ls = 0; }
      }
   }
}


void
dmz::RenderPluginLinkOSG::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LinkStruct *ls = _linkTable.remove (LinkHandle);

   if (ls) {

      if (ls->root.valid () && _render) {

         osg::ref_ptr<osg::Group> scene = _render->lookup_dynamic_object (ls->Link);

         if (scene.valid ()) {

            scene->removeChild (ls->root.get ());
            _render->destroy_dynamic_object (ls->Link);
         }
      }

      delete ls; ls = 0;
   }
}


void
dmz::RenderPluginLinkOSG::update_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle,
      const UUID &AttributeIdentity,
      const Handle AttributeObjectHandle,
      const UUID &PrevAttributeIdentity,
      const Handle PrevAttributeObjectHandle) {

   LinkStruct *ls = _linkTable.lookup (LinkHandle);

   if (ls) {

      if (PrevAttributeObjectHandle) { _attrTable.remove (PrevAttributeObjectHandle); }

      if (AttributeObjectHandle) {

         _attrTable.store (AttributeObjectHandle, ls);

         ObjectModule *module (get_object_module ());

         if (module) {

            ls->hide = module->lookup_flag (AttributeObjectHandle, _hideAttrHandle);

            if (ls->Def.ScaleAttrHandle) {

               module->lookup_scalar (
                  AttributeObjectHandle,
                  ls->Def.ScaleAttrHandle,
                  ls->scale);
            }

            _update_link (*ls);
         }
      }
   }
}


void
dmz::RenderPluginLinkOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _hideAttrHandle) {

      ObjectStruct *os = _objTable.lookup (ObjectHandle);

      if (os) {

         os->hide = Value;
         _update_links (*os);
      }
      else {

         LinkStruct *ls = _attrTable.lookup (ObjectHandle);

         if (ls) {

            ls->hide = Value;
            _update_link (*ls);
         }
      }
   }
}


void
dmz::RenderPluginLinkOSG::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os = _objTable.lookup (ObjectHandle);

   if (os) {

      os->pos = Value;
      _update_links (*os);
   }
}


void
dmz::RenderPluginLinkOSG::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   ObjectStruct *os = _objTable.lookup (ObjectHandle);

   if (os) {

      os->ori = Value;
      os->offset = os->Center;
      Value.transform_vector (os->offset);
      _update_links (*os);
   }
}


void
dmz::RenderPluginLinkOSG::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   LinkStruct *ls = _attrTable.lookup (ObjectHandle);

   if (ls) {

      ls->scale = Value;

      _update_link (*ls);
   }
}


// RenderPluginLinkOSG Interface
dmz::RenderPluginLinkOSG::ObjectStruct *
dmz::RenderPluginLinkOSG::_lookup_object (const Handle Object) {

   ObjectStruct *result = _objTable.lookup (Object);

   if (!result) {

      Vector center;

      ObjectModule *module = get_object_module ();

      if (module) {

         ObjectType type = module->lookup_object_type (Object);

         Vector *ptr = _centerTable.lookup (type.get_handle ());

         if (!ptr) { 

            Config data = type.find_config ("center-offset");

            if (data) { ptr = new Vector (config_to_vector (data)); }
            else { ptr = new Vector; }

            if (ptr && !_centerTable.store (type.get_handle (), ptr)) {

               delete ptr; ptr = 0;
            }
         }

         if (ptr) { center = *ptr; }
      }

      result = new ObjectStruct (Object, center);

      if (result && _objTable.store (Object, result)) { 

         if (module) {

            module->lookup_position (Object, _defaultAttrHandle, result->pos);
            module->lookup_orientation (Object, _defaultAttrHandle, result->ori);
            result->offset = result->Center;
            result->ori.transform_vector (result->offset);

         }
      }
      else if (result) { delete result; result = 0; }
   }

   return result;
}


void
dmz::RenderPluginLinkOSG::_create_link (LinkStruct &ls) {

   if (_render) {

      ls.root = new osg::MatrixTransform;

      ls.root->setUserData (new RenderObjectDataOSG (ls.Link));
      ls.root->setDataVariance (osg::Object::DYNAMIC);
      ls.root->addChild (ls.Def.model.get ());

      ls.root->setNodeMask (
         (ls.root->getNodeMask () & ~_masterMask) |
            (ls.Def.Glyph ? _glyphMask : _entityMask));

      osg::ref_ptr<osg::Group> scene = _render->create_dynamic_object (ls.Link);

      if (scene.valid ()) { scene->addChild (ls.root.get ()); }

      _update_link (ls);
   }
}


void
dmz::RenderPluginLinkOSG::_update_link (LinkStruct &ls) {

   if (ls.root.valid ()) {

      UInt32 mask = ls.root->getNodeMask ();

      if (ls.hide || ls.sub.hide || ls.super.hide) { mask &= ~_cullMask; }
      else { mask |= _cullMask; }

      ls.root->setNodeMask (mask);

      Vector dir = (ls.super.pos + ls.super.offset) - (ls.sub.pos + ls.sub.offset);
      const Vector Scale (ls.scale, ls.scale, dir.magnitude ());
      dir.normalize_in_place ();
      Matrix rot (dir);

      ls.root->setMatrix (to_osg_matrix (rot, ls.sub.pos + ls.sub.offset, Scale));
   }
}


void
dmz::RenderPluginLinkOSG::_update_links (ObjectStruct &os) {

   HashTableHandleIterator it;
   LinkStruct *link (0);

   while (os.superTable.get_next (it, link)) { _update_link (*link); }
   it.reset ();
   link = 0;
   while (os.subTable.get_next (it, link)) { _update_link (*link); }
}


void
dmz::RenderPluginLinkOSG::_create_geometry (LinkDefStruct &def) {

   def.model = new osg::Geode ();

   osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back (def.Color);
   geom->setColorArray (colors);
   geom->setColorBinding (osg::Geometry::BIND_OVERALL);

   osg::StateSet *stateset = def.model->getOrCreateStateSet ();
   stateset->setAttributeAndModes (new osg::CullFace (osg::CullFace::BACK));

#if 0
   osg::ref_ptr<osg::Material> material = new osg::Material;
   material->setEmission (osg::Material::FRONT_AND_BACK, def.Color);
   stateset->setAttributeAndModes (material.get (), osg::StateAttribute::ON);
#endif

   osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
   osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

   osg::ref_ptr<osg::Vec3Array> top (0);
   osg::ref_ptr<osg::Vec3Array> bottom (0);
   osg::ref_ptr<osg::Geometry> topGeom;
   osg::ref_ptr<osg::Geometry> bottomGeom;

   if (def.Capped) {

      top = new osg::Vec3Array;
      bottom = new osg::Vec3Array;

      topGeom = new osg::Geometry;
      bottomGeom = new osg::Geometry;

      topGeom->setColorArray (colors);
      topGeom->setColorBinding (osg::Geometry::BIND_OVERALL);

      bottomGeom->setColorArray (colors);
      bottomGeom->setColorBinding (osg::Geometry::BIND_OVERALL);
   }

   const Vector Offset (0.0, 0.0, -1.0);
   const Vector Forward (0.0, 0.0, -1.0);

   Float64 Angle = TwoPi64 / (Float64 (def.Sides));

   int count (0);
   int capCount (0);

   for (Int32 ix = 0; ix <= def.Sides; ix++) {

      Matrix xform;
      Vector vec (0.0, 1.0, 0.0);

      if ((ix > 0) && (ix < def.Sides)) {

         Matrix xform (Forward, Angle * (Float64)ix);
         xform.transform_vector (vec);
      }

      Vector radius (vec * def.Radius);

      vertices->push_back (to_osg_vector (radius + Offset)); count++;
      vertices->push_back (to_osg_vector (radius)); count++;
      normals->push_back (to_osg_vector (vec));
      normals->push_back (to_osg_vector (vec));

      if (top.valid ()) { top->push_back (to_osg_vector (radius + Offset)); }

      if (bottom.valid ()) {

         bottom->insert (bottom->begin (), to_osg_vector (radius));
      }

      capCount++;
   }

   geom->setNormalArray (normals);
   geom->setNormalBinding (osg::Geometry::BIND_PER_VERTEX);
   geom->addPrimitiveSet (new osg::DrawArrays (GL_TRIANGLE_STRIP, 0, count));
   geom->setVertexArray (vertices);

   def.model->addDrawable (geom.get ());

   if (top && topGeom.valid ()) {

      osg::ref_ptr<osg::Vec3Array> topNorm = new osg::Vec3Array;
      topNorm->push_back (osg::Vec3 (0.0, 0.0, -1.0));
      topGeom->setNormalArray (topNorm);
      topGeom->setNormalBinding (osg::Geometry::BIND_OVERALL);
      topGeom->addPrimitiveSet (new osg::DrawArrays (GL_TRIANGLE_FAN, 0, capCount - 1));
      topGeom->setVertexArray (top.get ());
      def.model->addDrawable (topGeom.get ());
   }

   if (bottom && bottomGeom.valid ()) {

      osg::ref_ptr<osg::Vec3Array> bottomNorm = new osg::Vec3Array;
      bottomNorm->push_back (osg::Vec3 (0.0, 0.0, 1.0));
      bottomGeom->setNormalArray (bottomNorm);
      bottomGeom->setNormalBinding (osg::Geometry::BIND_OVERALL);

      bottomGeom->addPrimitiveSet (
         new osg::DrawArrays (GL_TRIANGLE_FAN, 0, capCount - 1));

      bottomGeom->setVertexArray (bottom.get ());
      def.model->addDrawable (bottomGeom.get ());
   }
}


void
dmz::RenderPluginLinkOSG::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   Config list;

   if (local.lookup_all_config ("link", list)) {

      ConfigIterator it;
      Config link;

      while (list.get_next_config (it, link)) {

         const Handle Attr = config_to_named_handle (
            "attribute",
            link,
            context);

         const Handle ScaleAttr = config_to_named_handle (
            "scale-attribute",
            link,
            context);

         const osg::Vec4 Color = config_to_osg_vec4_color (
            link,
            osg::Vec4 (1.0, 1.0, 1.0, 1.0));

         const Float64 Radius = config_to_float64 ("radius", link, 0.15);

         const Int32 Sides = config_to_int32 ("sides", link, 4);

         const Boolean Glyph = config_to_boolean ("glyph", link, True);

         const Boolean Capped = config_to_boolean ("capped", link, False);

         LinkDefStruct *lds = new LinkDefStruct (
            Attr,
            ScaleAttr,
            Color,
            Radius,
            Sides,
            Glyph,
            Capped);

         if (lds && _defTable.store (Attr, lds)) {

            activate_object_attribute (
               Attr,
               ObjectLinkMask | ObjectUnlinkMask | ObjectLinkAttributeMask);

            if (ScaleAttr) {

               activate_object_attribute (ScaleAttr, ObjectScalarMask);
            }
         }
         else if (lds) { delete lds; lds = 0; }
      }
   }

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectDestroyMask | ObjectPositionMask | ObjectOrientationMask);

   _hideAttrHandle = activate_object_attribute (ObjectAttributeHideName, ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginLinkOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginLinkOSG (Info, local);
}

};
