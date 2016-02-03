/* GDK - The GIMP Drawing Kit
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors: William Hua <william.hua@canonical.com>
 */

#ifndef __GDK_ATTACH_PARAMS_PRIVATE_H__
#define __GDK_ATTACH_PARAMS_PRIVATE_H__

#include "gdkattachparams.h"

G_BEGIN_DECLS

/**
 * GdkAttachParams:
 * @parent_instance: parent instance
 * @has_attach_rect: %TRUE if @attach_rect is valid
 * @attach_rect: the attachment rectangle to attach the window to
 * @rect_parent: the #GdkWindow that @attach_rect is relative to
 * @rect_anchor: the anchoring point on @attach_rect
 * @window_anchor: the anchoring point on the window
 * @attach_hints: positioning hints for the backend
 * @offset_x: the horizontal offset to displace the window by
 * @offset_y: the vertical offset to displace the window by
 * @position_callback: a function to call when the final position is known
 * @callback_user_data: additional data to pass to @position_callback
 * @user_data_destroy_notify: a function to free @callback_user_data
 *
 * Opaque type containing the information needed to position a window relative
 * to an attachment rectangle.
 *
 * Since: 3.20
 */
struct _GdkAttachParams
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  gboolean has_attach_rect;
  GdkRectangle attach_rect;
  GdkWindow *rect_parent;

  GdkAttachAnchor rect_anchor;
  GdkAttachAnchor window_anchor;
  GdkAttachHints attach_hints;

  gint offset_x;
  gint offset_y;

  GdkAttachCallback position_callback;
  gpointer callback_user_data;
  GDestroyNotify user_data_destroy_notify;
};

void gdk_attach_params_default_move_window (GdkAttachParams *params,
                                            GdkWindow       *window);

G_END_DECLS

#endif /* __GDK_ATTACH_PARAMS_PRIVATE_H__ */
