#include "config.h"

#include "gdkattachparamsprivate.h"
#include "gdkscreen.h"
#include "gdkwindow.h"

/**
 * SECTION: gdkattachparams
 * @section_id: gdkattachparams
 * @title: Attachment Parameters
 * @short_description: Describing relative window position
 * @stability: Unstable
 * @include: gdk/gdkattachparams.h
 *
 * A full description of how a window should be positioned relative to an
 * attachment rectangle.
 *
 * Certain widgets such as menus and combo boxes don't require explicit
 * absolute positioning; they only need to be aligned with respect to another
 * anchoring widget, such as a menu item, in such a way to not overflow
 * off-screen. GTK+ cannot always determine such an optimal position since it
 * requires knowledge of the geometry of the monitor workarea as well as the
 * ability to position windows in absolute root window coordinates, which some
 * GDK backends do not support.
 *
 * A minimal #GdkAttachParams description should have an attachment rectangle,
 * a list of primary constraints, and a list of secondary constraints. The
 * attachment rectangle is the allocation of the anchoring widget, which can
 * be a menu item, menu button, combo box, etc. It can even be a 1x1 pixel at
 * the current cursor position. The primary constraints are a list of
 * #GdkAttachRules in descending priority that the GDK backend can try in order
 * to fix the position of the window either horizontally or vertically. The
 * secondary constraints are a list of #GdkAttachRules in descending priority
 * to fix the position of the window on the other axis.
 *
 * ![](gdkattachrule.png)
 *
 * A #GdkAttachRule is a bit mask that constrains the position of a window on a
 * single axis: %GDK_ATTACH_AXIS_X or %GDK_ATTACH_AXIS_Y. It must also specify
 * one of %GDK_ATTACH_RECT_MIN, %GDK_ATTACH_RECT_MID, or %GDK_ATTACH_RECT_MAX,
 * and one of %GDK_ATTACH_WINDOW_MIN, %GDK_ATTACH_WINDOW_MID, or
 * %GDK_ATTACH_WINDOW_MAX. The backend will make a best effort to align the
 * window position to the attachment rectangle position on that axis, or move
 * on to the next constraint if available.
 *
 * ![](gdkattachparams.png)
 *
 * There are also additional parameters that can be set to fine tune the
 * positioning of the window, such as margins and paddings, as well as a
 * callback to obtain the final position of the window.
 *
 * Since: 3.20
 */

/**
 * gdk_attach_params_new:
 *
 * Creates a new #GdkAttachParams for describing the position of a #GdkWindow
 * relative to an attachment #GdkRectangle.
 *
 * Returns: (transfer full): a new #GdkAttachParams, to be freed with
 *          gdk_attach_params_free()
 *
 * Since: 3.20
 */
GdkAttachParams *
gdk_attach_params_new (void)
{
  GdkAttachParams *params = g_new0 (GdkAttachParams, 1);

  params->primary_rules = g_array_new (TRUE, TRUE, sizeof (GdkAttachRule));
  params->secondary_rules = g_array_new (TRUE, TRUE, sizeof (GdkAttachRule));

  return params;
}

/**
 * gdk_attach_params_copy:
 * @src: the #GdkAttachParams to copy
 * @data: (nullable): unused
 *
 * Creates a deep copy of @src.
 *
 * Returns: (transfer full) (nullable): a deep copy of @src, to be freed with
 *          gdk_attach_params_free()
 *
 * Since: 3.20
 */
gpointer
gdk_attach_params_copy (gconstpointer src,
                        gpointer      data)
{
  GdkAttachParams *copy;
  const GdkAttachParams *params;

  if (!src)
    return NULL;

  params = src;

  copy = g_memdup (params, sizeof (*params));

  copy->primary_rules = g_array_sized_new (TRUE, TRUE, sizeof (GdkAttachRule),
                                           params->primary_rules->len);

  g_array_append_vals (copy->primary_rules,
                       params->primary_rules->data,
                       params->primary_rules->len);

  copy->secondary_rules = g_array_sized_new (TRUE, TRUE, sizeof (GdkAttachRule),
                                             params->secondary_rules->len);

  g_array_append_vals (copy->secondary_rules,
                       params->secondary_rules->data,
                       params->secondary_rules->len);

  return copy;
}

/**
 * gdk_attach_params_free:
 * @data: the #GdkAttachParams to free
 *
 * Releases @data.
 *
 * Since: 3.20
 */
void
gdk_attach_params_free (gpointer data)
{
  GdkAttachParams *params;

  g_return_if_fail (data);

  params = data;

  if (params->attach_user_data && params->attach_destroy_notify)
    params->attach_destroy_notify (params->attach_user_data);

  g_array_unref (params->secondary_rules);
  g_array_unref (params->primary_rules);

  g_free (params);
}

/**
 * gdk_attach_params_set_attach_origin:
 * @params: a #GdkAttachParams
 * @x: x-coordinate of the attachment rectangle's origin
 * @y: y-coordinate of the attachment rectangle's origin
 *
 * Sets the origin of the attachment rectangle's coordinate system in root
 * coordinates.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_attach_origin (GdkAttachParams *params,
                                     gint             x,
                                     gint             y)
{
  g_return_if_fail (params);

  params->origin_x = x;
  params->origin_y = y;
}

/**
 * gdk_attach_params_set_attach_rect:
 * @params: a #GdkAttachParams
 * @rectangle: (nullable): the attachment rectangle
 *
 * Sets the attachment rectangle the window needs to be aligned relative to.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_attach_rect (GdkAttachParams    *params,
                                   const GdkRectangle *rectangle)
{
  g_return_if_fail (params);

  if (rectangle)
    {
      params->has_attach_rect = TRUE;
      params->attach_rect = *rectangle;
    }
  else
    params->has_attach_rect = FALSE;
}

/**
 * gdk_attach_params_set_attach_margin:
 * @params: a #GdkAttachParams
 * @margin: (nullable): the space around the attachment rectangle
 *
 * Sets the amount of space to leave around the attachment rectangle.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_attach_margin (GdkAttachParams *params,
                                     const GdkBorder *margin)
{
  GdkBorder zero = { 0 };

  g_return_if_fail (params);

  params->attach_margin = margin ? *margin : zero;
}

/**
 * gdk_attach_params_set_window_margin:
 * @params: a #GdkAttachParams
 * @margin: (nullable): the space around the window
 *
 * Sets the amount of space to leave around the window.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_window_margin (GdkAttachParams *params,
                                     const GdkBorder *margin)
{
  GdkBorder zero = { 0 };

  g_return_if_fail (params);

  params->window_margin = margin ? *margin : zero;
}

/**
 * gdk_attach_params_set_window_padding:
 * @params: a #GdkAttachParams
 * @padding: (nullable): the space between the window and its
 *           contents.
 *
 * Sets the amount of space between the window and its contents.
 *
 * Since: 3.20
 */
void
gdk_attach_params_set_window_padding (GdkAttachParams *params,
                                      const GdkBorder *padding)
{
  GdkBorder zero = { 0 };

  g_return_if_fail (params);

  params->window_padding = padding ? *padding : zero;
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
  g_return_if_fail (params);

  params->offset_x = x;
  params->offset_y = y;
}

static void
add_rules (GArray        *array,
           GdkAttachRule  first_rule,
           va_list        args)
{
  GdkAttachRule rule;

  for (rule = first_rule; rule; rule = va_arg (args, GdkAttachRule))
    g_array_append_val (array, rule);
}

/**
 * gdk_attach_params_add_primary_rules_valist:
 * @params: a #GdkAttachParams
 * @first_rule: first primary rule
 * @args: a #va_list of the remaining primary rules
 *
 * Non-variadic version of gdk_attach_params_add_primary_rules().
 *
 * Since: 3.20
 */
void
gdk_attach_params_add_primary_rules_valist (GdkAttachParams *params,
                                            GdkAttachRule    first_rule,
                                            va_list          args)
{
  add_rules (params->primary_rules, first_rule, args);
}

/**
 * gdk_attach_params_add_secondary_rules_valist:
 * @params: a #GdkAttachParams
 * @first_rule: first secondary rule
 * @args: a #va_list of the remaining secondary rules
 *
 * Non-variadic version of gdk_attach_params_add_secondary_rules().
 *
 * Since: 3.20
 */
void
gdk_attach_params_add_secondary_rules_valist (GdkAttachParams *params,
                                              GdkAttachRule    first_rule,
                                              va_list          args)
{
  add_rules (params->secondary_rules, first_rule, args);
}

/**
 * gdk_attach_params_add_primary_rules:
 * @params: a #GdkAttachParams
 * @first_rule: first primary rule
 * @...: a %NULL-terminated list of rules
 *
 * Appends to the list of primary positioning rules to try.
 *
 * A typical backend will try each primary rule in the order they're added. If
 * a rule can be satisfied, it will then try each secondary rule until it
 * finds a satisfiable secondary rule that doesn't conflict with the primary
 * rule. If it finds a pair of satisfiable non-conflicting rules, then it will
 * place the window there. If it cannot find a pair, it proceeds to the next
 * primary rule and tries again.
 *
 * Since: 3.20
 */
void
gdk_attach_params_add_primary_rules (GdkAttachParams *params,
                                     GdkAttachRule    first_rule,
                                     ...)
{
  va_list args;

  g_return_if_fail (params);

  va_start (args, first_rule);

  gdk_attach_params_add_primary_rules_valist (params, first_rule, args);

  va_end (args);
}

/**
 * gdk_attach_params_add_secondary_rules:
 * @params: a #GdkAttachParams
 * @first_rule: first secondary rule
 * @...: a %NULL-terminated list of rules
 *
 * Appends to the list of secondary positioning rules to try.
 *
 * A typical backend will try each primary rule in the order they're added. If
 * a rule can be satisfied, it will then try each secondary rule until it
 * finds a satisfiable secondary rule that doesn't conflict with the primary
 * rule. If it finds a pair of satisfiable non-conflicting rules, then it will
 * place the window there. If it cannot find a pair, it proceeds to the next
 * primary rule and tries again.
 *
 * Since: 3.20
 */
void
gdk_attach_params_add_secondary_rules (GdkAttachParams *params,
                                       GdkAttachRule    first_rule,
                                       ...)
{
  va_list args;

  g_return_if_fail (params);

  va_start (args, first_rule);

  gdk_attach_params_add_secondary_rules_valist (params, first_rule, args);

  va_end (args);
}

static void
rules_foreach (GArray   *array,
               GFunc     func,
               gpointer  user_data)
{
  gint i;

  for (i = 0; i < array->len; i++)
    func (&g_array_index (array, GdkAttachRule, i), user_data);
}

/**
 * gdk_attach_params_primary_rules_foreach:
 * @params: a #GdkAttachParams
 * @func: the function to call with each primary rule
 * @user_data: (nullable): user data to pass to the function
 *
 * Applies @func to each primary rule in @params.
 *
 * Since: 3.20
 */
void
gdk_attach_params_primary_rules_foreach (GdkAttachParams *params,
                                         GFunc            func,
                                         gpointer         user_data)
{
  g_return_if_fail (params);
  g_return_if_fail (func);

  rules_foreach (params->primary_rules, func, user_data);
}

/**
 * gdk_attach_params_secondary_rules_foreach:
 * @params: a #GdkAttachParams
 * @func: the function to call with each secondary rule
 * @user_data: (nullable): user data to pass to the function
 *
 * Applies @func to each secondary rule in @params.
 *
 * Since: 3.20
 */
void
gdk_attach_params_secondary_rules_foreach (GdkAttachParams *params,
                                           GFunc            func,
                                           gpointer         user_data)
{
  g_return_if_fail (params);
  g_return_if_fail (func);

  rules_foreach (params->secondary_rules, func, user_data);
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
  g_return_if_fail (params);

  params->attach_callback = callback;

  if (user_data != params->attach_user_data)
    {
      if (params->attach_user_data && params->attach_destroy_notify)
        params->attach_destroy_notify (params->attach_user_data);

      params->attach_user_data = user_data;
      params->attach_destroy_notify = destroy_notify;
    }
  else if (user_data)
    g_warning ("%s (): params already owns user data", G_STRFUNC);
}

static gboolean
is_satisfiable (GdkAttachRule          rule,
                const GdkAttachParams *params,
                gint                   width,
                gint                   height,
                const GdkRectangle    *bounds,
                gint                  *value)
{
  gboolean use_rect_margin;
  gboolean use_window_margin;
  gboolean use_window_padding;

  g_return_val_if_fail (params, FALSE);
  g_return_val_if_fail (params->has_attach_rect, FALSE);

  use_rect_margin = (((rule & GDK_ATTACH_RECT_MASK)   == GDK_ATTACH_RECT_MIN &&
                      (rule & GDK_ATTACH_WINDOW_MASK) == GDK_ATTACH_WINDOW_MAX) ||
                     ((rule & GDK_ATTACH_RECT_MASK)   == GDK_ATTACH_RECT_MAX &&
                      (rule & GDK_ATTACH_WINDOW_MASK) == GDK_ATTACH_WINDOW_MIN));

  use_window_margin = use_rect_margin;

  use_window_padding = TRUE;

  switch (rule & GDK_ATTACH_AXIS_MASK)
    {
    case GDK_ATTACH_AXIS_X:
      *value = params->origin_x;

      switch (rule & GDK_ATTACH_RECT_MASK)
        {
        case GDK_ATTACH_RECT_MIN:
          *value += params->attach_rect.x;

          if (use_rect_margin)
            *value -= params->attach_margin.left;

          break;

        case GDK_ATTACH_RECT_MID:
          *value += params->attach_rect.x + params->attach_rect.width / 2;
          break;

        case GDK_ATTACH_RECT_MAX:
          *value += params->attach_rect.x + params->attach_rect.width;

          if (use_rect_margin)
            *value += params->attach_margin.right;

          break;
        }

      switch (rule & GDK_ATTACH_WINDOW_MASK)
        {
        case GDK_ATTACH_WINDOW_MIN:
          if (use_window_margin)
            *value += params->window_margin.left;

          if (use_window_padding)
            *value -= params->window_margin.left;

          break;

        case GDK_ATTACH_WINDOW_MID:
          *value -= width / 2;
          break;

        case GDK_ATTACH_WINDOW_MAX:
          *value -= width;

          if (use_window_margin)
            *value -= params->window_margin.right;

          if (use_window_padding)
            *value += params->window_margin.right;

          break;
        }

      *value += params->offset_x;

      return !bounds || (bounds->x <= *value && *value + width <= bounds->x + bounds->width);

    case GDK_ATTACH_AXIS_Y:
      *value = params->origin_y;

      switch (rule & GDK_ATTACH_RECT_MASK)
        {
        case GDK_ATTACH_RECT_MIN:
          *value += params->attach_rect.y;

          if (use_rect_margin)
            *value -= params->attach_margin.top;

          break;

        case GDK_ATTACH_RECT_MID:
          *value += params->attach_rect.y + params->attach_rect.height / 2;
          break;

        case GDK_ATTACH_RECT_MAX:
          *value += params->attach_rect.y + params->attach_rect.height;

          if (use_rect_margin)
            *value += params->attach_margin.bottom;

          break;
        }

      switch (rule & GDK_ATTACH_WINDOW_MASK)
        {
        case GDK_ATTACH_WINDOW_MIN:
          if (use_window_margin)
            *value += params->window_margin.top;

          if (use_window_padding)
            *value -= params->window_margin.top;

          break;

        case GDK_ATTACH_WINDOW_MID:
          *value -= height / 2;
          break;

        case GDK_ATTACH_WINDOW_MAX:
          *value -= height;

          if (use_window_margin)
            *value -= params->window_margin.bottom;

          if (use_window_padding)
            *value += params->window_margin.bottom;

          break;
        }

      *value += params->offset_y;

      return !bounds || (bounds->y <= *value && *value + height <= bounds->y + bounds->height);
    }

  return FALSE;
}

#define BEST      0
#define GOOD      1
#define PRIMARY   0
#define SECONDARY 1
#define X         0
#define Y         1

/**
 * gdk_attach_params_choose_position:
 * @params: a #GdkAttachParams
 * @width: window width
 * @height: window height
 * @bounds: (nullable): monitor geometry
 * @x: (out) (optional): the best x-coordinate for the window
 * @y: (out) (optional): the best y-coordinate for the window
 * @offset_x: (out) (optional): the horizontal displacement needed to push the
 *            window on-screen
 * @offset_y: (out) (optional): the vertical displacement needed to push the
 *            window on-screen
 * @primary_rule: (out) (optional): the best primary rule
 * @secondary_rule: (out) (optional): the best secondary rule
 *
 * Finds the best position for a window of size @width and @height on a screen
 * with @bounds using the given @params.
 *
 * Returns: %TRUE if there is a pair of satisfiable primary and secondary
 *          rules that do not conflict with each other
 *
 * Since: 3.20
 */
gboolean
gdk_attach_params_choose_position (const GdkAttachParams *params,
                                   gint                   width,
                                   gint                   height,
                                   const GdkRectangle    *bounds,
                                   gint                  *x,
                                   gint                  *y,
                                   gint                  *offset_x,
                                   gint                  *offset_y,
                                   GdkAttachRule         *primary_rule,
                                   GdkAttachRule         *secondary_rule)
{
  gint tmp_x;
  gint tmp_y;
  gint tmp_offset_x;
  gint tmp_offset_y;
  GdkAttachRule pr;
  GdkAttachRule sr;
  GdkAttachRule rules[2][2][2] = { 0 };
  gint axes[2][2] = { 0 };
  gint values[2][2][2] = { 0 };
  GArray *arrays[2];
  gint i;
  gint j;
  gint k;
  GdkAttachRule rule;
  gint axis;
  gint value;
  gboolean satisfiable;
  gboolean success;

  g_return_val_if_fail (params, FALSE);
  g_return_val_if_fail (params->has_attach_rect, FALSE);

  if (!x)
    x = &tmp_x;

  if (!y)
    y = &tmp_y;

  if (!offset_x)
    offset_x = &tmp_offset_x;

  if (!offset_y)
    offset_y = &tmp_offset_y;

  if (!primary_rule)
    primary_rule = &pr;

  if (!secondary_rule)
    secondary_rule = &sr;

  arrays[PRIMARY] = params->primary_rules;
  arrays[SECONDARY] = params->secondary_rules;

  for (i = PRIMARY; i <= SECONDARY; i++)
    {
      for (j = 0; j < arrays[i]->len; j++)
        {
          rule = g_array_index (arrays[i], GdkAttachRule, j);

          switch (rule & GDK_ATTACH_AXIS_MASK)
            {
            case GDK_ATTACH_AXIS_X:
              axis = X;
              break;

            case GDK_ATTACH_AXIS_Y:
              axis = Y;
              break;

            default:
              axis = -1;
              break;
            }

          if (axis < 0)
            {
              g_warning ("%s (): invalid constraint axis: 0x%X", G_STRFUNC, rule);
              continue;
            }

          satisfiable = is_satisfiable (rule, params, width, height, bounds, &value);

          if (satisfiable && !rules[i][BEST][axis])
            {
              rules[i][BEST][axis] = rule;
              values[i][BEST][axis] = value;

              if (rules[i][BEST][!axis])
                break;
              else
                axes[i][BEST] = axis;
            }
          else if (!rules[i][GOOD][axis])
            {
              rules[i][GOOD][axis] = rule;
              values[i][GOOD][axis] = value;

              if (!rules[i][GOOD][!axis])
                axes[i][GOOD] = axis;
            }
        }
    }

  success = FALSE;

  for (i = BEST; i <= GOOD; i++)
    {
      for (j = BEST; j <= GOOD; j++)
        {
          for (k = Y; k >= X; k--)
            {
              if (rules[PRIMARY][i][axes[PRIMARY][i] == k] && rules[SECONDARY][j][axes[PRIMARY][i] != k])
                {
                  *primary_rule = rules[PRIMARY][i][axes[PRIMARY][i] == k];
                  *secondary_rule = rules[SECONDARY][j][axes[PRIMARY][i] != k];

                  if ((axes[PRIMARY][i] == k) == X)
                    {
                      *x = values[PRIMARY][i][X];
                      *y = values[SECONDARY][j][Y];
                    }
                  else
                    {
                      *x = values[SECONDARY][j][X];
                      *y = values[PRIMARY][i][Y];
                    }

                  *offset_x = 0;
                  *offset_y = 0;
                  success = TRUE;
                  break;
                }
            }

          if (success)
            break;
        }

      if (success)
        break;
    }

  if (success && bounds)
    {
      if (*x + width > bounds->x + bounds->width)
        {
          *offset_x += bounds->x + bounds->width - width - *x;
          *x = bounds->x + bounds->width - width;
        }

      if (*x < bounds->x)
        {
          *offset_x += bounds->x - *x;
          *x = bounds->x;
        }

      if (*y + height > bounds->y + bounds->height)
        {
          *offset_y += bounds->y + bounds->height - height - *y;
          *y = bounds->y + bounds->height - height;
        }

      if (*y < bounds->y)
        {
          *offset_y += bounds->y - *y;
          *y = bounds->y;
        }
    }

  return success;
}

/**
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
 * @primary_rule: (out) (optional): the best primary rule
 * @secondary_rule: (out) (optional): the best secondary rule
 *
 * Finds the best position for @window according to @params.
 *
 * Returns: %TRUE if there's a best position
 *
 * Since: 3.20
 */
gboolean
gdk_attach_params_choose_position_for_window (const GdkAttachParams *params,
                                              GdkWindow             *window,
                                              gint                  *x,
                                              gint                  *y,
                                              gint                  *offset_x,
                                              gint                  *offset_y,
                                              GdkAttachRule         *primary_rule,
                                              GdkAttachRule         *secondary_rule)
{
  GdkScreen *screen;
  gint center_x;
  gint center_y;
  gint monitor;
  GdkRectangle bounds;
  gint width;
  gint height;

  g_return_val_if_fail (params, FALSE);
  g_return_val_if_fail (params->has_attach_rect, FALSE);
  g_return_val_if_fail (window, FALSE);

  screen = gdk_window_get_screen (window);
  center_x = params->origin_x + params->attach_rect.x + params->attach_rect.width / 2;
  center_y = params->origin_y + params->attach_rect.y + params->attach_rect.height / 2;
  monitor = gdk_screen_get_monitor_at_point (screen, center_x, center_y);
  gdk_screen_get_monitor_workarea (screen, monitor, &bounds);
  width = gdk_window_get_width (window);
  height = gdk_window_get_height (window);

  return gdk_attach_params_choose_position (params,
                                            width,
                                            height,
                                            &bounds,
                                            x,
                                            y,
                                            offset_x,
                                            offset_y,
                                            primary_rule,
                                            secondary_rule);
}

/**
 * gdk_attach_params_move_window:
 * @params: a #GdkAttachParams
 * @window: (transfer none) (not nullable): the #GdkWindow to position
 *
 * Moves @window to the best position according to @params.
 *
 * Since: 3.20
 */
void
gdk_attach_params_move_window (const GdkAttachParams *params,
                               GdkWindow             *window)
{
  gint x;
  gint y;
  gint offset_x;
  gint offset_y;
  GdkAttachRule primary_rule;
  GdkAttachRule secondary_rule;

  g_return_if_fail (GDK_IS_WINDOW (window));

  if (!params || !params->has_attach_rect)
    return;

  if (gdk_attach_params_choose_position_for_window (params,
                                                    window,
                                                    &x,
                                                    &y,
                                                    &offset_x,
                                                    &offset_y,
                                                    &primary_rule,
                                                    &secondary_rule))
    {
      gdk_window_move (window, x, y);

      if (params->attach_callback)
        params->attach_callback (window,
                                 params,
                                 x,
                                 y,
                                 offset_x,
                                 offset_y,
                                 primary_rule,
                                 secondary_rule,
                                 params->attach_user_data);
    }
}
