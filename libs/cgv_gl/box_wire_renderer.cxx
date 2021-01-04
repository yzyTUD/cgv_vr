#include "box_wire_renderer.h"
#include <cgv_gl/gl/gl.h>
#include <cgv_gl/gl/gl_tools.h>

namespace cgv {
	namespace render {
		box_wire_render_style::box_wire_render_style()
		{
			default_extent = vec3(1.0f);
			relative_anchor = vec3(0.0f);
		}

		box_wire_renderer& ref_box_wire_renderer(context& ctx, int ref_count_change)
		{
			static int ref_count = 0;
			static box_wire_renderer r;
			r.manage_singelton(ctx, "box_wire_renderer", ref_count, ref_count_change);
			return r;
		}

		/// overload to allow instantiation of box_wire_renderer
		render_style* box_wire_renderer::create_render_style() const
		{
			return new line_render_style();
		}

		box_wire_renderer::box_wire_renderer()
		{
			has_extents = false;
			position_is_center = true;
			has_translations = false;
			has_rotations = false;
		}
		void box_wire_renderer::set_attribute_array_manager(const context& ctx, attribute_array_manager* _aam_ptr)
		{
			line_renderer::set_attribute_array_manager(ctx, _aam_ptr);
			if (aam_ptr) {
				if (aam_ptr->has_attribute(ctx, ref_prog().get_attribute_location(ctx, "extent")))
					has_extents = true;
				if (aam_ptr->has_attribute(ctx, ref_prog().get_attribute_location(ctx, "translation")))
					has_translations = true;
				if (aam_ptr->has_attribute(ctx, ref_prog().get_attribute_location(ctx, "rotation")))
					has_rotations = true;
			}
			else {
				has_extents = false;
				has_translations = false;
				has_rotations = false;
			}
		}
		/// set the flag, whether the position is interpreted as the box center
		void box_wire_renderer::set_position_is_center(bool _position_is_center)
		{
			position_is_center = _position_is_center;
		}


		bool box_wire_renderer::init(context& ctx)
		{
			bool res = renderer::init(ctx);
			if (!ref_prog().is_created()) {
				if (!ref_prog().build_program(ctx, "box_wire.glpr", true)) {
					std::cerr << "ERROR in box_wire_renderer::init() ... could not build program box_wire.glpr" << std::endl;
					return false;
				}
			}
			return res;
		}
		/// 
		bool box_wire_renderer::enable(context& ctx)
		{
			if (!line_renderer::enable(ctx))
				return false;
			ref_prog().set_uniform(ctx, "position_is_center", position_is_center);
			ref_prog().set_uniform(ctx, "has_rotations", has_rotations);
			ref_prog().set_uniform(ctx, "has_translations", has_translations);
			const auto& brs = get_style<box_wire_render_style>();
			ref_prog().set_uniform(ctx, "relative_anchor", brs.relative_anchor);
			if (!has_extents)
				ref_prog().set_attribute(ctx, "extent", brs.default_extent);
			return true;
		}
		///
		bool box_wire_renderer::disable(context& ctx)
		{
			if (!attributes_persist()) {
				has_extents = false;
				position_is_center = true;
				has_rotations = false;
				has_translations = false;
			}

			return line_renderer::disable(ctx);
		}
		
		void box_wire_renderer::draw(context& ctx, size_t start, size_t count, bool use_strips, bool use_adjacency, uint32_t strip_restart_index)
		{
			draw_impl(ctx, PT_POINTS, start, count, false, false, -1);
		}
		bool box_wire_render_style_reflect::self_reflect(cgv::reflect::reflection_handler& rh)
		{
			return
				rh.reflect_base(*static_cast<line_render_style*>(this)) &&
				rh.reflect_member("default_extent", default_extent) &&
				rh.reflect_member("relative_anchor", relative_anchor);
		}


		cgv::reflect::extern_reflection_traits<box_wire_render_style, box_wire_render_style_reflect> get_reflection_traits(const box_wire_render_style&)
		{
			return cgv::reflect::extern_reflection_traits<box_wire_render_style, box_wire_render_style_reflect>();
		}
	}
}

#include <cgv/gui/provider.h>

namespace cgv {
	namespace gui {

		struct box_wire_render_style_gui_creator : public gui_creator
		{
			/// attempt to create a gui and return whether this was successful
			bool create(provider* p, const std::string& label,
				void* value_ptr, const std::string& value_type,
				const std::string& gui_type, const std::string& options, bool*)
			{
				if (value_type != cgv::type::info::type_name<cgv::render::box_wire_render_style>::get_name())
					return false;
				cgv::render::box_wire_render_style* brs_ptr = reinterpret_cast<cgv::render::box_wire_render_style*>(value_ptr);
				cgv::base::base* b = dynamic_cast<cgv::base::base*>(p);
				p->add_member_control(b, "default_extent.x", brs_ptr->default_extent[0], "value_slider", "min=0.001;max=1000;log=true;ticks=true");
				p->add_member_control(b, "default_extent.y", brs_ptr->default_extent[1], "value_slider", "min=0.001;max=1000;log=true;ticks=true");
				p->add_member_control(b, "default_extent.z", brs_ptr->default_extent[2], "value_slider", "min=0.001;max=1000;log=true;ticks=true");
				p->add_member_control(b, "relative_anchor.x", brs_ptr->relative_anchor[0], "value_slider", "min=-1;max=1;ticks=true");
				p->add_member_control(b, "relative_anchor.y", brs_ptr->relative_anchor[1], "value_slider", "min=-1;max=1;ticks=true");
				p->add_member_control(b, "relative_anchor.z", brs_ptr->relative_anchor[2], "value_slider", "min=-1;max=1;ticks=true");
				p->add_gui("line_render_style", *static_cast<cgv::render::line_render_style*>(brs_ptr));
				return true;
			}
		};

#include "gl/lib_begin.h"

		CGV_API cgv::gui::gui_creator_registration<box_wire_render_style_gui_creator> box_wire_rs_gc_reg("box_wire_render_style_gui_creator");
	}
}