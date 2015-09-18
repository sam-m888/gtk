#ifndef __GDK_ATTACH_PARAMS_PRIVATE_H__
#define __GDK_ATTACH_PARAMS_PRIVATE_H__

#include "gdkattachparams.h"

G_BEGIN_DECLS

/*
 * GdkAttachParams:
 * @origin_x: x-coordinate of root origin of @attach_rect coordinate system
 * @origin_y: y-coordinate of root origin of @attach_rect coordinate system
 * @has_attach_rect: %TRUE if @attach_rect is valid
 * @attach_rect: the attachment rectangle to attach the window to
 * @attach_margin: the space to leave around @attach_rect
 * @window_margin: the space to leave around the window
 * @window_padding: the space between the window and its contents
 * @offset_x: the horizontal offset to displace the window by
 * @offset_y: the vertical offset to displace the window by
 * @primary_rules: an array of primary #GdkAttachRule
 * @secondary_rules: an array of secondary #GdkAttachRule
 * @position_callback: a function to call when the final position is known
 * @position_callback_user_data: additional data to pass to @position_callback
 * @position_callback_destroy_notify: a function to free
 *                                    @position_callback_user_data
 *
 * Opaque type containing the information needed to position a window relative
 * to an attachment rectangle.
 *
 * Since: 3.20
 */
struct _GdkAttachParams
{
  /*< private >*/
  gint origin_x;
  gint origin_y;

  gboolean has_attach_rect;
  GdkRectangle attach_rect;

  GdkBorder attach_margin;
  GdkBorder window_margin;
  GdkBorder window_padding;

  gint offset_x;
  gint offset_y;

  GArray *primary_rules;
  GArray *secondary_rules;

  GdkAttachCallback attach_callback;
  gpointer attach_user_data;
  GDestroyNotify attach_destroy_notify;
};

G_GNUC_INTERNAL
gboolean gdk_attach_params_choose_position            (const GdkAttachParams *params,
                                                       gint                   width,
                                                       gint                   height,
                                                       const GdkRectangle    *bounds,
                                                       gint                  *x,
                                                       gint                  *y,
                                                       gint                  *offset_x,
                                                       gint                  *offset_y,
                                                       GdkAttachRule         *primary_rule,
                                                       GdkAttachRule         *secondary_rule);

G_GNUC_INTERNAL
gboolean gdk_attach_params_choose_position_for_window (const GdkAttachParams *params,
                                                       GdkWindow             *window,
                                                       gint                  *x,
                                                       gint                  *y,
                                                       gint                  *offset_x,
                                                       gint                  *offset_y,
                                                       GdkAttachRule         *primary_rule,
                                                       GdkAttachRule         *secondary_rule);

G_GNUC_INTERNAL
void     gdk_attach_params_move_window                (const GdkAttachParams *params,
                                                       GdkWindow             *window);

G_END_DECLS

#endif /* __GDK_ATTACH_PARAMS_PRIVATE_H__ */
