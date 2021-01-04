#include "spline_tube_renderer.h"
#include <cgv_gl/gl/gl.h>
#include <cgv_gl/gl/gl_tools.h>

namespace cgv {
	namespace render {
		spline_tube_renderer& ref_spline_tube_renderer(context& ctx, int ref_count_change)
		{
			static int ref_count = 0;
			static spline_tube_renderer r;
			r.manage_singelton(ctx, "spline_tube_renderer", ref_count, ref_count_change);
			return r;
		}

		render_style* spline_tube_renderer::create_render_style() const
		{
			return new spline_tube_render_style();
		}

		spline_tube_render_style::spline_tube_render_style()
		{
			radius_scale = 1.0f;
			radius = 1.0f;
		}

		spline_tube_renderer::spline_tube_renderer()
		{
			has_radii = false;
			has_tangents = false;
		}

		void spline_tube_renderer::set_attribute_array_manager(const context& ctx, attribute_array_manager* _aam_ptr)
		{
			surface_renderer::set_attribute_array_manager(ctx, _aam_ptr);
			if(aam_ptr) {
				if(aam_ptr->has_attribute(ctx, ref_prog().get_attribute_location(ctx, "radius")))
					has_radii = true;
				if(aam_ptr->has_attribute(ctx, ref_prog().get_attribute_location(ctx, "tangent")))
					has_tangents = true;
			} else {
				has_radii = false;
				has_tangents = false;
			}
		}

		bool spline_tube_renderer::validate_attributes(const context& ctx) const
		{
			const spline_tube_render_style& strs = get_style<spline_tube_render_style>();
			bool res = surface_renderer::validate_attributes(ctx);
			return res;
		}
		bool spline_tube_renderer::init(cgv::render::context& ctx)
		{
			bool res = surface_renderer::init(ctx);
			if(!ref_prog().is_created()) {
				if(!ref_prog().build_program(ctx, "spline_tube.glpr", true)) {
					std::cerr << "ERROR in spline_tube_renderer::init() ... could not build program spline_tube.glpr" << std::endl;
					return false;
				}
			}
			return res;
		}

		/// 
		bool spline_tube_renderer::enable(context& ctx)
		{
			const spline_tube_render_style& strs = get_style<spline_tube_render_style>();

			if(!surface_renderer::enable(ctx))
				return false;

			if(!ref_prog().is_linked())
				return false;

			if(!has_radii)
				ref_prog().set_attribute(ctx, "radius", strs.radius);

			if(!has_tangents)
				ref_prog().set_attribute(ctx, "tangent", vec4(0.0));

			ref_prog().set_uniform(ctx, "radius_scale", strs.radius_scale);
			ref_prog().set_uniform(ctx, "eye_pos", eye_pos);

			return true;
		}
		///
		bool spline_tube_renderer::disable(context& ctx)
		{
			if(!attributes_persist()) {
				has_radii = false;
				has_tangents = false;
			}
			return surface_renderer::disable(ctx);
		}

		bool spline_tube_render_style_reflect::self_reflect(cgv::reflect::reflection_handler& rh)
		{
			return
				rh.reflect_base(*static_cast<spline_tube_render_style*>(this)) &&
				rh.reflect_member("radius", radius);
				rh.reflect_member("radius_scale", radius_scale);
		}

		void spline_tube_renderer::draw(context& ctx, size_t start, size_t count, bool use_strips, bool use_adjacency, uint32_t strip_restart_index)
		{
			draw_impl(ctx, PT_LINES, start, count, use_strips, use_adjacency, strip_restart_index);
		}

		cgv::reflect::extern_reflection_traits<spline_tube_render_style, spline_tube_render_style_reflect> get_reflection_traits(const spline_tube_render_style&)
		{
			return cgv::reflect::extern_reflection_traits<spline_tube_render_style, spline_tube_render_style_reflect>();
		}
	}
}

#include <cgv/gui/provider.h>

namespace cgv {
	namespace gui {

		struct spline_tube_render_style_gui_creator : public gui_creator {
			/// attempt to create a gui and return whether this was successful
			bool create(provider* p, const std::string& label,
				void* value_ptr, const std::string& value_type,
				const std::string& gui_type, const std::string& options, bool*) {
				if(value_type != cgv::type::info::type_name<cgv::render::spline_tube_render_style>::get_name())
					return false;
				cgv::render::spline_tube_render_style* strs_ptr = reinterpret_cast<cgv::render::spline_tube_render_style*>(value_ptr);
				cgv::base::base* b = dynamic_cast<cgv::base::base*>(p);

				p->add_member_control(b, "default radius", strs_ptr->radius, "value_slider", "min=0.001;step=0.0001;max=10.0;log=true;ticks=true");
				p->add_member_control(b, "radius scale", strs_ptr->radius_scale, "value_slider", "min=0.01;step=0.0001;max=100.0;log=true;ticks=true");
				
				p->add_gui("surface_render_style", *static_cast<cgv::render::surface_render_style*>(strs_ptr));
				return true;
			}
		};

#include "gl/lib_begin.h"

		CGV_API cgv::gui::gui_creator_registration<spline_tube_render_style_gui_creator> spline_tube_rs_gc_reg("spline_tube_render_style_gui_creator");
	}
}
