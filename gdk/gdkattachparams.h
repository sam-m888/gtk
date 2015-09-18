#ifndef __GDK_ATTACH_PARAMS_H__
#define __GDK_ATTACH_PARAMS_H__

#if !defined (__GDK_H_INSIDE__) && !defined (GDK_COMPILATION)
#error "Only <gdk/gdk.h> can be included directly."
#endif

#include <gdk/gdktypes.h>
#include <gdk/gdkversionmacros.h>

G_BEGIN_DECLS

/**
 * GdkAttachRule:
 * @GDK_ATTACH_UNKNOWN: not a constraint
 * @GDK_ATTACH_AXIS_X: constrain horizontally
 * @GDK_ATTACH_AXIS_Y: constrain vertically
 * @GDK_ATTACH_AXIS_MASK: mask for constraint axis
 * @GDK_ATTACH_RECT_MIN: left/top edge of rectangle
 * @GDK_ATTACH_RECT_MID: center of rectangle
 * @GDK_ATTACH_RECT_MAX: right/bottom edge of rectangle
 * @GDK_ATTACH_RECT_MASK: mask for rectangle anchor
 * @GDK_ATTACH_WINDOW_MIN: left/top edge of window
 * @GDK_ATTACH_WINDOW_MID: center of window
 * @GDK_ATTACH_WINDOW_MAX: right/bottom edge of window
 * @GDK_ATTACH_WINDOW_MASK: mask for window anchor
 *
 * Constraints on the position of the window relative to its attachment
 * rectangle.
 *
 * ![](gdkattachrule.png)
 *
 * Since: 3.20
 */
typedef enum _GdkAttachRule
{
  GDK_ATTACH_UNKNOWN     = 0,
  GDK_ATTACH_AXIS_X      = 1 << 0,
  GDK_ATTACH_AXIS_Y      = 1 << 1,
  GDK_ATTACH_AXIS_MASK   = GDK_ATTACH_AXIS_X | GDK_ATTACH_AXIS_Y,
  GDK_ATTACH_RECT_MIN    = 1 << 2,
  GDK_ATTACH_RECT_MID    = 2 << 2,
  GDK_ATTACH_RECT_MAX    = 3 << 2,
  GDK_ATTACH_RECT_MASK   = GDK_ATTACH_RECT_MIN | GDK_ATTACH_RECT_MID | GDK_ATTACH_RECT_MAX,
  GDK_ATTACH_WINDOW_MIN  = 1 << 4,
  GDK_ATTACH_WINDOW_MID  = 2 << 4,
  GDK_ATTACH_WINDOW_MAX  = 3 << 4,
  GDK_ATTACH_WINDOW_MASK = GDK_ATTACH_WINDOW_MIN | GDK_ATTACH_WINDOW_MID | GDK_ATTACH_WINDOW_MAX
} GdkAttachRule;

/**
 * GdkAttachParams:
 *
 * Opaque type containing the information needed to position a window relative
 * to an attachment rectangle.
 *
 * Since: 3.20
 */
typedef struct _GdkAttachParams GdkAttachParams;

/**
 * GdkAttachCallback:
 * @window: the #GdkWindow that was moved
 * @params: (transfer none) (nullable): the #GdkAttachParams that was used
 * @x: the final x-coordinate of @window
 * @y: the final y-coordinate of @window
 * @offset_x: the horizontal displacement applied to keep @window on-screen
 * @offset_y: the vertical displacement applied to keep @window on-screen
 * @primary_rule: the primary rule that was used for positioning. If unknown,
 *                this will be %GDK_ATTACH_UNKNOWN
 * @secondary_rule: the secondary rule that was used for positioning. If
 *                  unknown, this will be %GDK_ATTACH_UNKNOWN
 * @user_data: (transfer none) (nullable): the user data that was set on
 *             @params
 *
 * A function that can be used to receive information about the final position
 * of a window.
 *
 * Since: 3.20
 */
typedef void (*GdkAttachCallback) (GdkWindow             *window,
                                   const GdkAttachParams *params,
                                   gint                   x,
                                   gint                   y,
                                   gint                   offset_x,
                                   gint                   offset_y,
                                   GdkAttachRule          primary_rule,
                                   GdkAttachRule          secondary_rule,
                                   gpointer               user_data);

GDK_AVAILABLE_IN_3_20
GdkAttachParams * gdk_attach_params_new                        (void);

GDK_AVAILABLE_IN_3_20
gpointer          gdk_attach_params_copy                       (gconstpointer          src,
                                                                gpointer               data);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_free                       (gpointer               data);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_attach_origin          (GdkAttachParams       *params,
                                                                gint                   x,
                                                                gint                   y);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_attach_rect            (GdkAttachParams       *params,
                                                                const GdkRectangle    *rectangle);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_attach_margin          (GdkAttachParams       *params,
                                                                const GdkBorder       *margin);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_window_margin          (GdkAttachParams       *params,
                                                                const GdkBorder       *margin);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_window_padding         (GdkAttachParams       *params,
                                                                const GdkBorder       *padding);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_window_offset          (GdkAttachParams       *params,
                                                                gint                   x,
                                                                gint                   y);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_add_primary_rules_valist   (GdkAttachParams       *params,
                                                                GdkAttachRule          first_rule,
                                                                va_list                args);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_add_secondary_rules_valist (GdkAttachParams       *params,
                                                                GdkAttachRule          first_rule,
                                                                va_list                args);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_add_primary_rules          (GdkAttachParams       *params,
                                                                GdkAttachRule          first_rule,
                                                                ...) G_GNUC_NULL_TERMINATED;

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_add_secondary_rules        (GdkAttachParams       *params,
                                                                GdkAttachRule          first_rule,
                                                                ...) G_GNUC_NULL_TERMINATED;

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_primary_rules_foreach      (GdkAttachParams       *params,
                                                                GFunc                  func,
                                                                gpointer               user_data);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_secondary_rules_foreach    (GdkAttachParams       *params,
                                                                GFunc                  func,
                                                                gpointer               user_data);

GDK_AVAILABLE_IN_3_20
void              gdk_attach_params_set_position_callback      (GdkAttachParams       *params,
                                                                GdkAttachCallback      callback,
                                                                gpointer               user_data,
                                                                GDestroyNotify         destroy_notify);

G_END_DECLS

#endif /* __GDK_ATTACH_PARAMS_H__ */
