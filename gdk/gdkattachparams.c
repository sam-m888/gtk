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

#include "config.h"

#include "gdkattachparamsprivate.h"
#include "gdkinternals.h"
#include "gdkscreen.h"
#include "gdkwindow.h"

/**
 * SECTION: gdkattachparams
 * @section_id: GdkAttachParams
 * @title: Attachment Parameters
 * @short_description: Relative window positioning
 * @stability: Unstable
 * @include: gdk/gdkattachparams.h
 *
 * A full description of how a window should be positioned relative to an
 * attachment rectangle.
 *
 * Certain widgets such as menus and combo boxes don't require explicit
 * positioning; they only need to be aligned with respect to another anchoring
 * widget, such as a menu item, in such a way to not overflow off-screen. GTK+
 * cannot always determine such an optimal position since it requires knowledge
 * of the geometry of the monitor work area as well as the ability to position
 * windows in absolute screen coordinates, which some GDK backends do not
 * support.
 *
 * ![](attach-menu.png)
 *
 * A minimal #GdkAttachParams description should have an attachment rectangle,
 * an attachment rectangle anchor, and a window anchor. The attachment
 * rectangle is typically the allocation of an anchoring widget, such as a menu
 * item, menu button, combo box, etc., but any arbitrary rectangle in any
 * window's coordinate system will do. It can even be a single pixel at the
 * master pointer, which is fairly common for context-sensitive menus.
 *
 * ![](attach-anchor.png)
 *
 * The attachment rectangle anchor is a #GdkAttachAnchor identifying a point on
 * the attachment rectangle that the window should be anchored to. The window
 * anchor is the point on the window that should anchor onto the attachment
 * rectangle's anchor.
 *
 * ![](attach-params.png)
 *
 * You can also specify additional positioning #GdkAttachHints that tell the
 * backend how to react if the preferred position is unavailable due to lack of
 * space. For example, if a drop-down combo box doesn't have enough space below
 * to show its menu without going off-screen, the backend can try placing it
 * above instead if the %GDK_ATTACH_FLIP_TOP_BOTTOM hint is set.
 *
 * ![](attach-flip-left-right.png)
 *
 * ![](attach-flip-top-bottom.png)
 *
 * There are also other parameters such as the window offset which can be used
 * to fine-tune the final position of the window, as well as a callback that
 * can be set to retrieve the final position as determined by the backend.
 *
 * Since: 3.20
 */

G_DEFINE_TYPE (GdkAttachParams, gdk_attach_params, G_TYPE_INITIALLY_UNOWNED)

static void
gdk_attach_params_dispose (GObject *object)
{
  GdkAttachParams *self = GDK_ATTACH_PARAMS (object);

  if (self->user_data_destroy_notify)
    g_clear_pointer (&self->callback_user_data, self->user_data_destroy_notify);

  g_clear_object (&self->rect_parent);

  G_OBJECT_CLASS (gdk_attach_params_parent_class)->dispose (object);
}

static void
gdk_attach_params_class_init (GdkAttachParamsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdk_attach_params_dispose;
}

static void
gdk_attach_params_init (GdkAttachParams *self)
{
  self->rect_anchor = GDK_ATTACH_CENTER;
  self->window_anchor = GDK_ATTACH_CENTER;
  self->attach_hints = GDK_ATTACH_FLIP_LEFT_RIGHT | GDK_ATTACH_FLIP_TOP_BOTTOM;
}

/**
 * gdk_attach_params_new:
 *
 * Creates a new #GdkAttachParams for describing the position of a window
 * relative to an attachment rectangle.
 *
 * Returns: (transfer floating): a new #GdkAttachParams
 *
 * Since: 3.20
 */
GdkAttachParams *
gdk_attach_params_new (void)
{
  return g_object_new (GDK_TYPE_ATTACH_PARAMS, NULL);
}

/**
 * gdk_attach_params_set_attach_rect:
 * @params: a #GdkAttachParams
 * @rectangle: (not nullable): the attachment rectangle
 * @parent: (not nullable): the #GdkWindow that @rectangle is relative to
 *
 * Sets the attachment rectangle the window needs to be aligned relative to.
 * The @rectangle should be in @parent's coordinate space.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_attach_rect (GdkAttachParams    *params,
                                   const GdkRectangle *rectangle,
                                   GdkWindow          *parent)
{
  gint x;
  gint y;

  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));
  g_return_if_fail (rectangle);
  g_return_if_fail (GDK_IS_WINDOW (parent));

  params->has_attach_rect = TRUE;
  params->attach_rect = *rectangle;

  while (!gdk_window_has_native (parent) && gdk_window_get_effective_parent (parent))
    {
      gdk_window_get_position (parent, &x, &y);
      params->attach_rect.x += x;
      params->attach_rect.y += y;

      parent = gdk_window_get_effective_parent (parent);
    }

  g_set_object (&params->rect_parent, parent);
}

/**
 * gdk_attach_params_has_attach_rect:
 * @params: a #GdkAttachParams
 *
 * Gets whether or not the attachment rectangle was set.
 *
 * Returns: %TRUE if the attachment rectangle was set
 *
 * Since: 3.20
 */
gboolean
gdk_attach_params_has_attach_rect (GdkAttachParams *params)
{
  g_return_val_if_fail (GDK_IS_ATTACH_PARAMS (params), FALSE);

  return params->has_attach_rect;
}

/**
 * gdk_attach_params_set_anchors:
 * @params: a #GdkAttachParams
 * @rect_anchor: the anchor on the attachment rectangle
 * @window_anchor: the anchor on the window
 *
 * Sets how the attachment rectangle and window should be anchored to each
 * other.
 *
 * ![](attach-anchor.png)
 *
 * ![](attach-params.png)
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_anchors (GdkAttachParams *params,
                               GdkAttachAnchor  rect_anchor,
                               GdkAttachAnchor  window_anchor)
{
  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));

  params->rect_anchor = rect_anchor;
  params->window_anchor = window_anchor;
}

/**
 * gdk_attach_params_get_anchors:
 * @params: a #GdkAttachParams
 * @rect_anchor: (out) (optional): the anchor on the attachment rectangle
 * @window_anchor: (out) (optional): the anchor on the window
 *
 * Gets how the attachment rectangle and window should be anchored to each
 * other.
 *
 * ![](attach-anchor.png)
 *
 * ![](attach-params.png)
 *
 * Since: 3.20
 */
void
gdk_attach_params_get_anchors (GdkAttachParams *params,
                               GdkAttachAnchor *rect_anchor,
                               GdkAttachAnchor *window_anchor)
{
  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));

  if (rect_anchor)
    *rect_anchor = params->rect_anchor;

  if (window_anchor)
    *window_anchor = params->window_anchor;
}

/**
 * gdk_attach_params_set_attach_hints:
 * @params: a #GdkAttachParams
 * @hints: positioning hints for the backend
 *
 * Sets positioning hints for the backend. For example,
 * %GDK_ATTACH_FLIP_LEFT_RIGHT means that the backend can try flipping the
 * anchors horizontally if not enough space is available in the preferred
 * direction.
 *
 * ![](attach-flip-left-right.png)
 *
 * ![](attach-flip-top-bottom.png)
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_attach_hints (GdkAttachParams *params,
                                    GdkAttachHints   hints)
{
  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));

  params->attach_hints = hints;
}

/**
 * gdk_attach_params_set_window_offset:
 * @params: a #GdkAttachParams
 * @x: horizontal displacement
 * @y: vertical displacement
 *
 * Sets the offset to displace the window by.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_window_offset (GdkAttachParams *params,
                                     gint             x,
                                     gint             y)
{
  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));

  params->offset_x = x;
  params->offset_y = y;
}

/**
 * gdk_attach_params_set_position_callback:
 * @params: a #GdkAttachParams
 * @callback: (nullable): a function to be called when the final position of
 *            the window is known
 * @user_data: (transfer full) (nullable): additional data to pass to @callback
 * @destroy_notify: (nullable): a function to release @user_data
 *
 * Sets the function to be called when the final position of the window is
 * known. Since the position might be determined asynchronously, don't assume
 * it will be called directly from gdk_window_move_using_params().
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_position_callback (GdkAttachParams   *params,
                                         GdkAttachCallback  callback,
                                         gpointer           user_data,
                                         GDestroyNotify     destroy_notify)
{
  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));

  params->position_callback = callback;

  if (user_data != params->callback_user_data)
    {
      if (params->callback_user_data && params->user_data_destroy_notify)
        params->user_data_destroy_notify (params->callback_user_data);

      params->callback_user_data = user_data;
      params->user_data_destroy_notify = destroy_notify;
    }
  else if (user_data)
    g_warning ("%s (): params already owns user data", G_STRFUNC);
}

static GdkAttachAnchor
get_opposite_anchor (GdkAttachAnchor anchor)
{
  switch (anchor & (GDK_ATTACH_LEFT | GDK_ATTACH_CENTER | GDK_ATTACH_RIGHT))
    {
    case GDK_ATTACH_LEFT:
      anchor = (anchor & ~GDK_ATTACH_LEFT) | GDK_ATTACH_RIGHT;
      break;

    default:
      g_warning ("%s (): invalid anchor 0x%x", G_STRFUNC, anchor);
    case GDK_ATTACH_CENTER:
      break;

    case GDK_ATTACH_RIGHT:
      anchor = (anchor & ~GDK_ATTACH_RIGHT) | GDK_ATTACH_LEFT;
      break;
    }

  switch (anchor & (GDK_ATTACH_TOP | GDK_ATTACH_CENTER | GDK_ATTACH_BOTTOM))
    {
    case GDK_ATTACH_TOP:
      anchor = (anchor & ~GDK_ATTACH_TOP) | GDK_ATTACH_BOTTOM;
      break;

    default:
      g_warning ("%s (): invalid anchor 0x%x", G_STRFUNC, anchor);
    case GDK_ATTACH_CENTER:
      break;

    case GDK_ATTACH_BOTTOM:
      anchor = (anchor & ~GDK_ATTACH_BOTTOM) | GDK_ATTACH_TOP;
      break;
    }

  return anchor;
}

static void
get_anchor_point (gint             x,
                  gint             y,
                  gint             width,
                  gint             height,
                  GdkAttachAnchor  anchor,
                  gint            *out_x,
                  gint            *out_y)
{
  if (out_x)
    {
      switch (anchor & (GDK_ATTACH_LEFT | GDK_ATTACH_CENTER | GDK_ATTACH_RIGHT))
        {
        case GDK_ATTACH_LEFT:
          *out_x = x;
          break;

        default:
          g_warning ("%s (): invalid anchor 0x%x", G_STRFUNC, anchor);
        case GDK_ATTACH_CENTER:
          *out_x = x + width / 2;
          break;

        case GDK_ATTACH_RIGHT:
          *out_x = x + width;
          break;
        }
    }

  if (out_y)
    {
      switch (anchor & (GDK_ATTACH_TOP | GDK_ATTACH_CENTER | GDK_ATTACH_BOTTOM))
        {
        case GDK_ATTACH_TOP:
          *out_y = y;
          break;

        default:
          g_warning ("%s (): invalid anchor 0x%x", G_STRFUNC, anchor);
        case GDK_ATTACH_CENTER:
          *out_y = y + height / 2;
          break;

        case GDK_ATTACH_BOTTOM:
          *out_y = y + height;
          break;
        }
    }
}

static gint
clamp_with_feedback (gint  val,
                     gint  min,
                     gint  max,
                     gint *offset)
{
  if (val < min && val > max)
    {
      if (min - val <= val - max)
        {
          *offset = min - val;
          return min;
        }
      else
        {
          *offset = max - val;
          return max;
        }
    }
  else if (val < min)
    {
      *offset = min - val;
      return min;
    }
  else if (val > max)
    {
      *offset = max - val;
      return max;
    }
  else
    {
      *offset = 0;
      return val;
    }
}

/*
 * gdk_attach_params_choose_position:
 * @params: a #GdkAttachParams
 * @width: window width
 * @height: window height
 * @top_shadow: size of top shadow
 * @left_shadow: size of left shadow
 * @right_shadow: size of right shadow
 * @bottom_shadow: size of bottom shadow
 * @bounds: (nullable): monitor geometry
 * @x: (out) (optional): the best x-coordinate for the window
 * @y: (out) (optional): the best y-coordinate for the window
 * @offset_x: (out) (optional): the horizontal displacement needed to push the
 *            window on-screen
 * @offset_y: (out) (optional): the vertical displacement needed to push the
 *            window on-screen
 * @flipped_x: (out) (optional): %TRUE if the window was flipped horizontally
 * @flipped_y: (out) (optional): %TRUE if the window was flipped vertically
 *
 * Finds the best position for a window of size @width and @height on a screen
 * with @bounds using the given @params.
 */
static void
gdk_attach_params_choose_position (GdkAttachParams    *params,
                                   gint                width,
                                   gint                height,
                                   gint                top_shadow,
                                   gint                left_shadow,
                                   gint                right_shadow,
                                   gint                bottom_shadow,
                                   const GdkRectangle *bounds,
                                   gint               *x,
                                   gint               *y,
                                   gint               *offset_x,
                                   gint               *offset_y,
                                   gboolean           *flipped_x,
                                   gboolean           *flipped_y)
{
  gint tmp_x;
  gint tmp_y;
  gint tmp_offset_x;
  gint tmp_offset_y;
  gboolean tmp_flipped_x;
  gboolean tmp_flipped_y;
  GdkWindow *parent;
  GdkRectangle padded_bounds;
  gint rect_x;
  gint rect_y;
  gint first_x;
  gint first_y;
  gint second_x;
  gint second_y;

  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));
  g_warn_if_fail (GDK_IS_WINDOW (params->rect_parent));
  g_return_if_fail (gdk_attach_params_has_attach_rect (params));

  if (!x)
    x = &tmp_x;

  if (!y)
    y = &tmp_y;

  if (!offset_x)
    offset_x = &tmp_offset_x;

  if (!offset_y)
    offset_y = &tmp_offset_y;

  if (!flipped_x)
    flipped_x = &tmp_flipped_x;

  if (!flipped_y)
    flipped_y = &tmp_flipped_y;

  *offset_x = 0;
  *offset_y = 0;
  *flipped_x = FALSE;
  *flipped_y = FALSE;

  parent = params->rect_parent ? params->rect_parent : gdk_get_default_root_window ();
  gdk_window_get_origin (parent, &rect_x, &rect_y);
  rect_x += params->attach_rect.x;
  rect_y += params->attach_rect.y;

  get_anchor_point (rect_x,
                    rect_y,
                    params->attach_rect.width,
                    params->attach_rect.height,
                    params->rect_anchor,
                    &first_x,
                    &first_y);

  get_anchor_point (first_x - left_shadow,
                    first_y - top_shadow,
                    -(width - left_shadow - right_shadow),
                    -(height - top_shadow - bottom_shadow),
                    params->window_anchor,
                    &first_x,
                    &first_y);

  first_x += params->offset_x;
  first_y += params->offset_y;

  if (bounds)
    {
      padded_bounds = *bounds;
      padded_bounds.x -= left_shadow;
      padded_bounds.y -= top_shadow;
      padded_bounds.width += left_shadow + right_shadow;
      padded_bounds.height += top_shadow + bottom_shadow;
    }

  if (bounds &&
      (params->attach_hints & GDK_ATTACH_FLIP_LEFT_RIGHT) &&
      (first_x < padded_bounds.x || first_x + width > padded_bounds.x + padded_bounds.width))
    {
      get_anchor_point (rect_x,
                        0,
                        params->attach_rect.width,
                        0,
                        get_opposite_anchor (params->rect_anchor),
                        &second_x,
                        NULL);

      get_anchor_point (second_x - left_shadow,
                        0,
                        -(width - left_shadow - right_shadow),
                        0,
                        get_opposite_anchor (params->window_anchor),
                        &second_x,
                        NULL);

      second_x -= params->offset_x;

      if (second_x >= padded_bounds.x && second_x + width <= padded_bounds.x + padded_bounds.width)
        {
          *x = second_x;
          *flipped_x = TRUE;
        }
      else
        *x = first_x;
    }
  else
    *x = first_x;

  if (bounds &&
      (params->attach_hints & GDK_ATTACH_FLIP_TOP_BOTTOM) &&
      (first_y < padded_bounds.y || first_y + height > padded_bounds.y + padded_bounds.height))
    {
      get_anchor_point (0,
                        rect_y,
                        0,
                        params->attach_rect.height,
                        get_opposite_anchor (params->rect_anchor),
                        NULL,
                        &second_y);

      get_anchor_point (0,
                        second_y - top_shadow,
                        0,
                        -(height - top_shadow - bottom_shadow),
                        get_opposite_anchor (params->window_anchor),
                        NULL,
                        &second_y);

      second_y -= params->offset_y;

      if (second_y >= padded_bounds.y && second_y + height <= padded_bounds.y + padded_bounds.height)
        {
          *y = second_y;
          *flipped_y = TRUE;
        }
      else
        *y = first_y;
    }
  else
    *y = first_y;

  if (bounds)
    {
      *x = clamp_with_feedback (*x, padded_bounds.x, padded_bounds.x + padded_bounds.width - width, offset_x);
      *y = clamp_with_feedback (*y, padded_bounds.y, padded_bounds.y + padded_bounds.height - height, offset_y);
    }
}

/*
 * gdk_attach_params_choose_position_for_window:
 * @params: a #GdkAttachParams
 * @window: (transfer none) (not nullable): the #GdkWindow to find the best
 *          position for
 * @x: (out) (optional): the best x-coordinate for the window
 * @y: (out) (optional): the best y-coordinate for the window
 * @offset_x: (out) (optional): the horizontal displacement needed to push the
 *            window on-screen
 * @offset_y: (out) (optional): the vertical displacement needed to push the
 *            window on-screen
 * @flipped_x: (out) (optional): %TRUE if the window was flipped horizontally
 * @flipped_y: (out) (optional): %TRUE if the window was flipped vertically
 *
 * Finds the best position for @window according to @params, assuming the
 * geometry of the monitor work area is available.
 */
static void
gdk_attach_params_choose_position_for_window (GdkAttachParams *params,
                                              GdkWindow       *window,
                                              gint            *x,
                                              gint            *y,
                                              gint            *offset_x,
                                              gint            *offset_y,
                                              gboolean        *flipped_x,
                                              gboolean        *flipped_y)
{
  GdkWindow *parent;
  GdkScreen *screen;
  gint origin_x;
  gint origin_y;
  gint center_x;
  gint center_y;
  gint monitor;
  GdkRectangle bounds;
  gint width;
  gint height;

  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));
  g_warn_if_fail (GDK_IS_WINDOW (params->rect_parent));
  g_return_if_fail (gdk_attach_params_has_attach_rect (params));
  g_return_if_fail (GDK_IS_WINDOW (window));

  parent = params->rect_parent ? params->rect_parent : gdk_get_default_root_window ();
  screen = gdk_window_get_screen (parent);
  gdk_window_get_origin (parent, &origin_x, &origin_y);
  center_x = origin_x + params->attach_rect.x + params->attach_rect.width / 2;
  center_y = origin_y + params->attach_rect.y + params->attach_rect.height / 2;
  monitor = gdk_screen_get_monitor_at_point (screen, center_x, center_y);
  gdk_screen_get_monitor_workarea (screen, monitor, &bounds);
  width = gdk_window_get_width (window);
  height = gdk_window_get_height (window);

  return gdk_attach_params_choose_position (params,
                                            width,
                                            height,
                                            window->top_shadow,
                                            window->left_shadow,
                                            window->right_shadow,
                                            window->bottom_shadow,
                                            &bounds,
                                            x,
                                            y,
                                            offset_x,
                                            offset_y,
                                            flipped_x,
                                            flipped_y);
}

/**
 * gdk_attach_params_default_move_window:
 * @params: a #GdkAttachParams
 * @window: (transfer none) (not nullable): the #GdkWindow to position
 *
 * Moves @window to the best position according to @params, assuming the
 * geometry of the monitor work area is available.
 *
 * Since: 3.20
 */
void
gdk_attach_params_default_move_window (GdkAttachParams *params,
                                       GdkWindow       *window)
{
  gint x;
  gint y;
  gint offset_x;
  gint offset_y;
  gboolean flipped_x;
  gboolean flipped_y;

  g_return_if_fail (GDK_IS_ATTACH_PARAMS (params));
  g_return_if_fail (gdk_attach_params_has_attach_rect (params));
  g_return_if_fail (GDK_IS_WINDOW (window));

  gdk_attach_params_choose_position_for_window (params,
                                                window,
                                                &x,
                                                &y,
                                                &offset_x,
                                                &offset_y,
                                                &flipped_x,
                                                &flipped_y);

  gdk_window_move (window, x, y);

  if (params->position_callback)
    params->position_callback (window,
                               params,
                               x,
                               y,
                               offset_x,
                               offset_y,
                               flipped_x,
                               flipped_y,
                               params->callback_user_data);
}
