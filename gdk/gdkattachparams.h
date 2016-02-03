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

#ifndef __GDK_ATTACH_PARAMS_H__
#define __GDK_ATTACH_PARAMS_H__

#if !defined (__GDK_H_INSIDE__) && !defined (GDK_COMPILATION)
#error "Only <gdk/gdk.h> can be included directly."
#endif

#include <gdk/gdktypes.h>
#include <gdk/gdkversionmacros.h>

G_BEGIN_DECLS

#define GDK_TYPE_ATTACH_PARAMS (gdk_attach_params_get_type ())

/**
 * GdkAttachParams:
 *
 * Opaque type containing the information needed to position a window relative
 * to an attachment rectangle.
 *
 * Since: 3.20
 */
GDK_AVAILABLE_IN_3_20
G_DECLARE_FINAL_TYPE (GdkAttachParams, gdk_attach_params, GDK, ATTACH_PARAMS, GInitiallyUnowned)

/**
 * GdkAttachAnchor:
 * @GDK_ATTACH_CENTER: center
 * @GDK_ATTACH_LEFT: midpoint of left edge
 * @GDK_ATTACH_RIGHT: midpoint of right edge
 * @GDK_ATTACH_TOP: midpoint of top edge
 * @GDK_ATTACH_BOTTOM: midpoint of bottom edge
 * @GDK_ATTACH_TOP_LEFT: top-left corner
 * @GDK_ATTACH_TOP_RIGHT: top-right corner
 * @GDK_ATTACH_BOTTOM_LEFT: bottom-left corner
 * @GDK_ATTACH_BOTTOM_RIGHT: bottom-right corner
 *
 * Flags for locating a special point on a rectangle.
 *
 * Since: 3.20
 */
typedef enum
{
  GDK_ATTACH_CENTER       = 0,
  GDK_ATTACH_LEFT         = 1 << 0,
  GDK_ATTACH_RIGHT        = 1 << 1,
  GDK_ATTACH_TOP          = 1 << 2,
  GDK_ATTACH_BOTTOM       = 1 << 3,
  GDK_ATTACH_TOP_LEFT     = GDK_ATTACH_TOP | GDK_ATTACH_LEFT,
  GDK_ATTACH_TOP_RIGHT    = GDK_ATTACH_TOP | GDK_ATTACH_RIGHT,
  GDK_ATTACH_BOTTOM_LEFT  = GDK_ATTACH_BOTTOM | GDK_ATTACH_LEFT,
  GDK_ATTACH_BOTTOM_RIGHT = GDK_ATTACH_BOTTOM | GDK_ATTACH_RIGHT
} GdkAttachAnchor;

/**
 * GdkAttachHints:
 * @GDK_ATTACH_NO_HINTS: the backend should try sliding the window back within
 *                       the monitor work area if not enough space is available
 * @GDK_ATTACH_FLIP_LEFT_RIGHT: the backend should try flipping the anchors
 *                              horizontally if not enough space is available
 * @GDK_ATTACH_FLIP_TOP_BOTTOM: the backend should try flipping the anchors
 *                              vertically if not enough space is available
 *
 * Positioning hints for the backend.
 *
 * Since: 3.20
 */
typedef enum
{
  GDK_ATTACH_NO_HINTS        = 0,
  GDK_ATTACH_FLIP_LEFT_RIGHT = 1 << 0,
  GDK_ATTACH_FLIP_TOP_BOTTOM = 1 << 1
} GdkAttachHints;

/**
 * GdkAttachCallback:
 * @window: the #GdkWindow that was moved
 * @params: (transfer none) (not nullable): the #GdkAttachParams that was used
 * @x: the final x-coordinate of @window in parent window coordinates
 * @y: the final y-coordinate of @window in parent window coordinates
 * @offset_x: how much horizontal displacement was applied to keep @window
 *            on-screen, relative to the originally intended position
 * @offset_y: how much vertical displacement was applied to keep @window
 *            on-screen, relative to the originally intended position
 * @flipped_x: %TRUE if the backend flipped @window horizontally
 * @flipped_y: %TRUE if the backend flipped @window vertically
 * @user_data: (transfer none) (nullable): the user data that was set on
 *             @params
 *
 * A function that can be used to receive information about the final position
 * of a window after gdk_window_move_using_params() is called. Since the
 * position might be determined asynchronously, don't assume it will be called
 * directly from gdk_window_move_using_params().
 *
 * Since: 3.20
 */
typedef void (*GdkAttachCallback) (GdkWindow       *window,
                                   GdkAttachParams *params,
                                   gint             x,
                                   gint             y,
                                   gint             offset_x,
                                   gint             offset_y,
                                   gboolean         flipped_x,
                                   gboolean         flipped_y,
                                   gpointer         user_data);

GDK_AVAILABLE_IN_3_20
GdkAttachParams * gdk_attach_params_new                   (void);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_attach_rect       (GdkAttachParams    *params,
                                                           const GdkRectangle *rectangle,
                                                           GdkWindow          *parent);

GDK_AVAILABLE_IN_3_20
gboolean          gdk_attach_params_has_attach_rect       (GdkAttachParams    *params);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_anchors           (GdkAttachParams    *params,
                                                           GdkAttachAnchor     rect_anchor,
                                                           GdkAttachAnchor     window_anchor);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_get_anchors           (GdkAttachParams    *params,
                                                           GdkAttachAnchor    *rect_anchor,
                                                           GdkAttachAnchor    *window_anchor);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_attach_hints      (GdkAttachParams    *params,
                                                           GdkAttachHints      hints);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_window_offset     (GdkAttachParams    *params,
                                                           gint                x,
                                                           gint                y);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_position_callback (GdkAttachParams    *params,
                                                           GdkAttachCallback   callback,
                                                           gpointer            user_data,
                                                           GDestroyNotify      destroy_notify);

G_END_DECLS

#endif /* __GDK_ATTACH_PARAMS_H__ */
