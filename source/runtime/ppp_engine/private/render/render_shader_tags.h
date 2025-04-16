#pragma once

#include "string/string_id.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        namespace unlit
        {
            struct color    {};
            struct texture  {};
            struct font     {};
            struct normal   {};
            struct shadow   {};
            struct predepth {};
        }
        //-------------------------------------------------------------------------
        namespace lit
        {
            struct color    {};
            struct texture  {};
            struct specular {};
        }

        //-------------------------------------------------------------------------
        template <typename T>
        struct tag;

        //-------------------------------------------------------------------------
        template <>
        struct tag<unlit::color>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("unlit_color");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_unlit_color");
                return id;
            }
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<unlit::texture>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("unlit_texture");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_unlit_texture");
                return id;
            }
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<unlit::font>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("unlit_font");
                return id;
            }

            // No instanced version of this shader
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<unlit::normal>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("unlit_normal");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_unlit_normal");
                return id;
            }
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<unlit::shadow>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("unlit_shadow");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_unlit_shadow");
                return id;
            }
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<unlit::predepth>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("unlit_predepth");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_unlit_predepth");
                return id;
            }
        };

        //-------------------------------------------------------------------------
        template <>
        struct tag<lit::color>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("lit_color");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_lit_color");
                return id;
            }
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<lit::texture>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("lit_texture");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_lit_texture");
                return id;
            }
        };
        //-------------------------------------------------------------------------
        template <>
        struct tag<lit::specular>
        {
            static string::string_id batched()
            {
                static string::string_id id = string::store_sid("lit_specular");
                return id;
            }
            static string::string_id instanced()
            {
                static string::string_id id = string::store_sid("instance_lit_specular");
                return id;
            }
        };

        //-------------------------------------------------------------------------
        namespace unlit
        {
            namespace tags
            {
                using color = tag<color>;
                using texture = tag<texture>;
                using font = tag<font>;
                using normal = tag<normal>;
                using shadow = tag<shadow>;
                using predepth = tag<predepth>;
            }
        }
        //-------------------------------------------------------------------------
        namespace lit
        {
            namespace tags
            {
                using color = tag<color>;
                using texture = tag<texture>;
                using specular = tag<specular>;
            }
        }
    }
}