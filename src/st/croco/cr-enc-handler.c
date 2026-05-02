/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 * This file is part of The Croco Library
 *
 * Copyright (C) 2002-2003 Dodji Seketeli <dodji@seketeli.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2.1 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/*
 *$Id$
 */

/**
 *@file
 *The definition of the #CREncHandler class.
 */

#include "cr-enc-handler.h"
#include "cr-utils.h"

#include <string.h>

struct CREncAlias {
        const gchar *name;
        enum CREncoding encoding;
};

static CREncHandler gv_default_enc_handlers[] = {
        {CR_UCS_1, cr_utils_ucs1_to_utf8, cr_utils_utf8_to_ucs1,
         cr_utils_ucs1_str_len_as_utf8, cr_utils_utf8_str_len_as_ucs1},

        {CR_ISO_8859_1, cr_utils_ucs1_to_utf8, cr_utils_utf8_to_ucs1,
         cr_utils_ucs1_str_len_as_utf8, cr_utils_utf8_str_len_as_ucs1},

        {CR_ASCII, cr_utils_ucs1_to_utf8, cr_utils_utf8_to_ucs1,
         cr_utils_ucs1_str_len_as_utf8, cr_utils_utf8_str_len_as_ucs1},

        {0, NULL, NULL, NULL, NULL}
};

/**
 * cr_enc_handler_get_instance:
 *@a_enc: the encoding of the Handler.
 *
 *Gets the instance of encoding handler.
 *This function implements a singleton pattern.
 *
 *Returns the instance of #CREncHandler.
 */
CREncHandler *
cr_enc_handler_get_instance (enum CREncoding a_enc)
{
        gulong i = 0;

        for (i = 0; gv_default_enc_handlers[i].encoding; i++) {
                if (gv_default_enc_handlers[i].encoding == a_enc) {
                        return (CREncHandler *) & gv_default_enc_handlers[i];
                }
        }

        return NULL;
}

/**
 * cr_enc_handler_convert_input:
 *@a_this: the current instance of #CREncHandler.
 *@a_in: the input buffer to convert.
 *@a_in_len: in/out parameter. The len of the input
 *buffer to convert. After return, contains the number of
 *bytes actually consumed.
 *@a_out: output parameter. The converted output buffer.
 *Must be freed by the buffer.
 *@a_out_len: output parameter. The length of the output buffer.
 *
 *Converts a raw input buffer into an utf8 buffer.
 *
 *Returns CR_OK upon successful completion, an error code otherwise.
 */
enum CRStatus
cr_enc_handler_convert_input (CREncHandler * a_this,
                              const guchar * a_in,
                              gulong * a_in_len,
                              guchar ** a_out, gulong * a_out_len)
{
        enum CRStatus status = CR_OK;

        g_return_val_if_fail (a_this && a_in && a_in_len && a_out,
                              CR_BAD_PARAM_ERROR);

        if (a_this->decode_input == NULL)
                return CR_OK;

        if (a_this->enc_str_len_as_utf8) {
                status = a_this->enc_str_len_as_utf8 (a_in,
                                                      &a_in[*a_in_len - 1],
                                                      a_out_len);

                g_return_val_if_fail (status == CR_OK, status);
        } else {
                *a_out_len = *a_in_len;
        }

        *a_out = g_malloc0 (*a_out_len);

        status = a_this->decode_input (a_in, a_in_len, *a_out, a_out_len);

        if (status != CR_OK) {
                g_free (*a_out);
                *a_out = NULL;
        }

        g_return_val_if_fail (status == CR_OK, status);

        return CR_OK;
}
