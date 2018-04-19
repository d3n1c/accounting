<?php

/**
 * Add body classes if certain regions have content.
 */
function dnccoreui_preprocess_html(&$variables) {
  // echo '<pre>'; print_r($variables); echo '</pre>'; exit;
  $variables['classes_array'][] = 'app';
  $variables['classes_array'][] = 'header-fixed';
  $variables['classes_array'][] = 'sidebar-fixed';
  $variables['classes_array'][] = 'aside-menu-fixed';
  $variables['classes_array'][] = 'aside-menu-hidden';

  $variables['head_title'] = htmlspecialchars_decode($variables['head_title']);
  $variables['head_title'] = strip_tags($variables['head_title']);

  // Add conditional stylesheets
  if (!module_exists('fontawesome')) {
    drupal_add_css(path_to_theme() . '/vendors/css/font-awesome.min.css');
  }
  drupal_add_css(path_to_theme() . '/vendors/css/simple-line-icons.min.css');
  drupal_add_css(path_to_theme() . '/css/style.css');

  // Add required js
  drupal_add_js(path_to_theme() . '/vendors/js/jquery.min.js', array(
    'scope' => 'footer',
    'weight' => -10,
  ));
  drupal_add_js(path_to_theme() . '/vendors/js/popper.min.js', array(
    'scope' => 'footer',
    'weight' => 10,
  ));
  drupal_add_js(path_to_theme() . '/vendors/js/bootstrap.min.js', array(
    'scope' => 'footer',
    'weight' => 20,
  ));
  drupal_add_js(path_to_theme() . '/vendors/js/pace.min.js', array(
    'scope' => 'footer',
    'weight' => 30,
  ));
  drupal_add_js(path_to_theme() . '/vendors/js/Chart.min.js', array(
    'scope' => 'footer',
    'weight' => 30,
  ));
  drupal_add_js(path_to_theme() . '/js/app.js', array(
    'scope' => 'footer',
    'weight' => 40,
  ));
  
  $pathnow = current_path();
  if ($pathnow == 'dnccoreui/examples/dashboard') {
    drupal_add_js(path_to_theme() . '/js/views/main.js', array(
      'scope' => 'footer',
      'weight' => 50,
    ));
  }
  unset ($pathnow);
  
  drupal_add_js(path_to_theme() . '/js/views/scopes.js', array(
    'scope' => 'footer',
    'weight' => 60,
  ));
}

/**
 * Override or insert variables into the page template for HTML output.
 */
function dnccoreui_process_html(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_html_alter($variables);
  }
}

/**
 * Override or insert variables into the page template.
 */
function dnccoreui_process_page(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_page_alter($variables);
  }
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
  // Since the title and the shortcut link are both block level elements,
  // positioning them next to each other is much simpler with a wrapper div.
  if (!empty($variables['title_suffix']['add_or_remove_shortcut']) && $variables['title']) {
    // Add a wrapper div using the title_prefix and title_suffix render elements.
    $variables['title_prefix']['shortcut_wrapper'] = array(
      '#markup' => '<div class="shortcut-wrapper clearfix">',
      '#weight' => 100,
    );
    $variables['title_suffix']['shortcut_wrapper'] = array(
      '#markup' => '</div>',
      '#weight' => -99,
    );
    // Make sure the shortcut link is the first item in title_suffix.
    $variables['title_suffix']['add_or_remove_shortcut']['#weight'] = -100;
  }

  // Add required css
  $t_logo = parse_url($variables['logo']);
  $t_logo = $t_logo['path'];
  $customcss = '
    .app-header.navbar .navbar-brand {
      background-image: url("' . $t_logo . '");
      background-size: ' . (basename($t_logo) == 'logo.png' ? 100 : 120) . 'px auto;
    }

    @media (min-width: 992px) {
      .brand-minimized .app-header.navbar .navbar-brand {
        background-image: url("' . base_path() . path_to_theme() . '/img/logo-symbol.png");
      }
    }
  ';
  drupal_add_css($customcss, array(
    'type' => 'inline'
  ));
  unset ($customcss);

  // manipulate header navigation
  if (!empty($variables['page']['header'])) {
    $ndump = $variables['page']['header'];
    foreach ($ndump as $keys => $values) {
      if (!empty($values['#markup'])) {
        $values['#markup'] = preg_replace('/<p[^>]*>/i', '', $values['#markup']);
        $values['#markup'] = preg_replace('/<\/p>/i', '', $values['#markup']);
      }
      $variables['page']['header'][$keys] = $values;
    }
    unset ($ndump);
  }

  if (!empty($variables['page']['navigation'])) {
    $ndump = $variables['page']['navigation'];
    foreach ($ndump as $keys => $values) {
      if (!empty($values['#markup'])) {
        $values['#markup'] = preg_replace('/<p[^>]*>/i', '', $values['#markup']);
        $values['#markup'] = preg_replace('/<\/p>/i', '', $values['#markup']);
      }
      $variables['page']['navigation'][$keys] = $values;
    }
    unset ($ndump);
  }

  $variables['title'] = htmlspecialchars_decode($variables['title']);
  $variables['title'] = strip_tags($variables['title']);

  global $user;
  if (!empty($user->uid)) {
    $user = user_load($user->uid);
    $variables['auth_user'] = (array)$user;
    $picture = !empty($user->picture->uri) ? $user->picture->uri : variable_get('user_picture_default');
    if (empty($picture)) {
      $variables['auth_user']['user_picture'] = '<img src="' . base_path() . path_to_theme() . '/img/logo-symbol.png" class="img-avatar" alt="' . $user->mail . '" />';
    }
    else {
      $variables['auth_user']['user_picture'] = theme('image_style', array(
        'style_name' => 'thumbnail',
        'path' => $picture,
        'attributes' => array(
          'class' => 'img-avatar',
          'alt' => $variables['auth_user']['mail'],
        ),
      ));
    }
    unset ($picture);
    $variables['auth_user']['menus'] = menu_navigation_links('user-menu');
  }

  $menus = [];
  foreach ($variables['page']['sidebar_first'] as $keys => $values) {
    if (!is_array($values)) {
      continue;
    }
    foreach ($values as $key => $value) {
      if (!is_numeric($key)) {
        continue;
      }
      if (is_array($value) && !empty($value['#theme'])) {
        if (preg_match('/^menu_link_/', $value['#theme'])) {
          $value['#title'] = (preg_match('~<i ~', $value['#title']) ? NULL : '<i class="icon-options-vertical"></i> ') . $value['#title'];
          $menus[$value['#original_link']['menu_name']][$key]['#title'] = $value['#title'];
          if (!empty($value['#below'])) {
            $menus[$value['#original_link']['menu_name']][$key]['#href'] = '#';
            $menus[$value['#original_link']['menu_name']][$key]['#children'] = [];
            $o = 0;
            foreach ($value['#below'] as $ke => $val) {
              if (!is_numeric($ke)) {
                continue;
              }
              $val['#title'] = $val['#title'] . (preg_match('~<i ~', $val['#title']) ? NULL : ' <i class="icon-options"></i>');
              $menus[$value['#original_link']['menu_name']][$key]['#children'][$o]['#title'] = $val['#title'];
              $menus[$value['#original_link']['menu_name']][$key]['#children'][$o]['#href'] = (preg_match('/^http/i', $val['#href']) ? NULL : base_path()) . $val['#href'];
              $o++;
            }
            unset($o);
          }
          else {
            $menus[$value['#original_link']['menu_name']][$key]['#href'] = (preg_match('/^http/i', $value['#href']) ? NULL : base_path()) . ($value['#href'] == '<front>' ? NULL : $value['#href']);
          }
        }
        else {
          if ($values['#block']->module == 'widget_factory') {
            $menus[$values['#block']->delta][$key]['#title'] = '<i class="' . $value['#item']['fa_icon'] . '"></i> ' . $value['#item']['title'];
            $menus[$values['#block']->delta][$key]['#href'] = $value['#item']['path'];
          }
        }
      }
    }
  }
  $variables['sidebar_first_menus'] = $menus;
  unset ($i, $menus);

  if (!empty($variables['tabs']['#primary'])) {
    $tdump = $variables['tabs']['#primary'];
    foreach ($tdump as $keys => $values) {
      $variables['tabs']['#primary'][$keys]['#link']['localized_options']['attributes']['class'][] = 'nav-link';
      if (!empty($values['#active'])) {
        $variables['tabs']['#primary'][$keys]['#link']['localized_options']['attributes']['class'][] = 'active';
      }
    }
    unset ($tdump);
  }

  $fdump = $variables['page']['footer'];
  foreach ($fdump as $keys => $values) {
    if (empty($values['#block'])) {
      continue;
    }
    if (!empty($values['#markup'])) {
      $values['#markup'] = preg_replace('/<p[^>]*>/i', '', $values['#markup']);
      $values['#markup'] = preg_replace('/<\/p>/i', '', $values['#markup']);
      $values['#markup'] = preg_replace('/<span[^>]*>/i', '', $values['#markup']);
      $values['#markup'] = preg_replace('/<\/span>/i', '', $values['#markup']);
      $variables['page']['footer'][$keys]['#markup'] = '<span>' . $values['#markup'] . '</span>';
      unset ($check, $dom);
    }
  }
  unset ($fdump);
}

/**
 * Implements hook_preprocess_maintenance_page().
 */
function dnccoreui_preprocess_maintenance_page(&$variables) {
  // By default, site_name is set to Drupal if no db connection is available
  // or during site installation. Setting site_name to an empty string makes
  // the site and update pages look cleaner.
  // @see template_preprocess_maintenance_page
  if (!$variables['db_is_active']) {
    $variables['site_name'] = '';
  }
  drupal_add_css(drupal_get_path('theme', 'bartik') . '/css/maintenance-page.css');
}

/**
 * Override or insert variables into the maintenance page template.
 */
function dnccoreui_process_maintenance_page(&$variables) {
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
}

/**
 * Override or insert variables into the node template.
 */
function dnccoreui_preprocess_node(&$variables) {
  if ($variables['view_mode'] == 'full' && node_is_page($variables['node'])) {
    $variables['classes_array'][] = 'node-full';
  }
}

/**
 * Override or insert variables into the block template.
 */
function dnccoreui_preprocess_block(&$variables) {
  // In the header region visually hide block titles.
  if ($variables['block']->region == 'header') {
    $variables['title_attributes_array']['class'][] = 'element-invisible';
  }
}

/**
 * Implements theme_menu_tree().
 */
function dnccoreui_menu_tree($variables) {
  return '<ul class="menu clearfix">' . $variables['tree'] . '</ul>';
}

function dnccoreui_html_head_alter(&$head_elements) {
  $head_elements['viewport'] = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
      'name' => 'viewport',
      'content' => 'width=device-width, initial-scale=1, shrink-to-fit=no',
    ),
    '#weight' => -9999,
  );
  drupal_add_html_head($head_elements, 'viewport');
}

/**
 * Implements theme_field__field_type().
 */
function dnccoreui_field__taxonomy_term_reference($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<h3 class="field-label">' . $variables['label'] . ': </h3>';
  }

  // Render the items.
  $output .= ($variables['element']['#label_display'] == 'inline') ? '<ul class="links inline">' : '<ul class="links">';
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<li class="taxonomy-term-reference-' . $delta . '"' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</li>';
  }
  $output .= '</ul>';

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . (!in_array('clearfix', $variables['classes_array']) ? ' clearfix' : '') . '"' . $variables['attributes'] .'>' . $output . '</div>';

  return $output;
}

/********Breadcrumbs*******/
/**
 * Overrides theme_breadcrumb().
 *
 * Print breadcrumbs as an ordered list.
 */
function dnccoreui_breadcrumb($variables) {
  $output = [];
  if (!empty($variables['breadcrumb'])) {
    $paths = [];
    foreach ($variables['breadcrumb'] as $keys => $values) {
      $values = htmlspecialchars_decode($values);
      $values = strip_tags($values, '<a>');
      libxml_use_internal_errors(true);
      $dom = new domdocument;
      $dom->loadHTML($values);
      $getpath = NULL;
      foreach ($dom->getElementsByTagName('a') as $a) {
        if (!empty($a->getAttribute('href'))) {
          $getpath = $a->getAttribute('href');
          if ($getpath == url('<front>')) {
            $values = '<a href="' . $getpath . '"><i class="icon-home"></i></a>';
          }
          break;
        }
      }
      if (empty($getpath) || in_array($getpath, $paths)) {
        unset ($getpath);
        continue;
      }
      $paths[] = $getpath;
      unset ($getpath);
      $output[] = $values;
    }
  }
  else {
    $output[] = '<i class="icon-home"></i>';
  }
  return $output;
}

function dnccoreui_preprocess_user_picture(&$variables) {
  unset ($variables['user_picture']);
  $userpict_toggle = variable_get('user_pictures', 0);
  if (!empty($userpict_toggle)) {
    $account = $variables['account'];
    if (!empty($account->picture)) {
      if (is_numeric($account->picture)) {
        $account->picture = file_load($account->picture);
      }
      if (!empty($account->picture->uri)) {
        $filepath = $account->picture->uri;
      }
    }
    elseif (variable_get('user_picture_default')) {
      $filepath = 'public://' . variable_get('user_picture_default');
    }
    if (isset($filepath)) {
      $alt = t("@user's picture", array(
        '@user' => format_username($account),
      ));

      // If the image does not have a valid Drupal scheme (for eg. HTTP),
      // don't load image styles.
      if (module_exists('image') && file_valid_uri($filepath) && ($style = variable_get('user_picture_style', ''))) {
        $variables['user_picture'] = theme('image_style', array(
          'style_name' => $style,
          'path' => $filepath,
          'alt' => $alt,
          'title' => $alt,
        ));
      }
      else {
        $variables['user_picture'] = theme('image', array(
          'path' => $filepath,
          'alt' => $alt,
          'title' => $alt,
        ));
      }
      if (!empty($account->uid) && user_access('access user profiles')) {
        $attributes = array(
          'attributes' => array(
            'title' => t('View user profile.'),
          ),
          'html' => TRUE,
        );
        $variables['user_picture'] = l($variables['user_picture'], 'user/' . $account->uid, $attributes);
      }
    }
    unset ($account);
  }
  unset ($userpict_toggle);
}

function dnccoreui_menu_local_tasks(&$variables) {
  $output = '';
  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="nav nav-tabs">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }
  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs secondary">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }
  return $output;
}

function dnccoreui_menu_local_task(&$variables) {
  $link = $variables['element']['#link'];
  $link_text = $link['title'];
  if (!empty($variables['element']['#active'])) {

    // Add text to indicate active tab for non-visual users.
    $active = '<span class="element-invisible">' . t('(active tab)') . '</span>';

    // If the link does not contain HTML already, check_plain() it now.
    // After we set 'html'=TRUE the link will not be sanitized by l().
    if (empty($link['localized_options']['html'])) {
      $link['title'] = check_plain($link['title']);
    }
    $link['localized_options']['html'] = TRUE;
    $link_text = t('!local-task-title!active', array(
      '!local-task-title' => $link['title'],
      '!active' => $active,
    ));
  }
  return '<li class="nav-item">' . l($link_text, $link['href'], $link['localized_options']) . "</li>\n";
}

/**
 * Returns HTML for a button form element.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #attributes, #button_type, #name, #value.
 *
 * @ingroup themeable
 */
function dnccoreui_button($variables) {
  $element = $variables['element'];
  $element['#attributes']['type'] = 'submit';
  element_set_attributes($element, array('id', 'name', 'value'));

  if (empty($element['#attributes']['class'])) {
    $element['#attributes']['class'][] = 'btn-primary';
  }
  else {
    foreach ($element['#attributes']['class'] as $value) {
      if (preg_match('/^btn-/i', $value)) {
        $itsokay = TRUE;
        break;
      }
    }
    if (empty($itsokay)) {
      $element['#attributes']['class'][] = 'btn-primary';
    }
    unset ($itsokay);
  }
  $element['#attributes']['class'][] = 'btn-sm';
  $element['#attributes']['class'][] = 'form-' . $element['#button_type'];

  return '<input' . drupal_attributes($element['#attributes']) . ' />';
}

/**
 * Implements hook_form_alter().
 */
function dnccoreui_form_alter(&$form, $form_state, $form_id) {
  // Change 'cancel' link to 'cancel' button.
  if ( $form['#theme'] == 'confirm_form' ) {
    if ($form['actions']['cancel']['#type'] == 'link') {
      $title = $form['actions']['cancel']['#title'];
      $href = $form['actions']['cancel']['#href'];
      // echo '<pre>'; print_r($form['actions']['cancel']); echo '</pre>'; exit;
      if (!is_null($title) and !is_null($href)) {
        // give space.
        $form['actions']['cancel']['#prefix'] = ' &nbsp; ';
      }
    }
  }
}

// function dnccoreui_preprocess_table(&$variables) {
//   $variables['attributes']['class'][] = 'table';
//   $variables['attributes']['class'][] = 'table-responsive-sm';
//   $variables['attributes']['class'][] = 'table-bordered';
//   $variables['attributes']['class'][] = 'table-striped';
//   $variables['attributes']['class'][] = 'table-sm';
// }

// function dnccoreui_table(&$variables) {
//   $header = $variables['header'];
//   $rows = $variables['rows'];
//   $attributes = $variables['attributes'];
//   $caption = $variables['caption'];
//   $colgroups = $variables['colgroups'];
//   $sticky = $variables['sticky'];
//   $empty = $variables['empty'];
//
//   // Add sticky headers, if applicable.
//   if (count($header) && $sticky) {
//     drupal_add_js('misc/tableheader.js');
//     // Add 'sticky-enabled' class to the table to identify it for JS.
//     // This is needed to target tables constructed by this function.
//     $attributes['class'][] = 'sticky-enabled';
//   }
//
//   $output = '<table' . drupal_attributes($attributes) . ">\n";
//
//   if (isset($caption)) {
//     $output .= '<caption>' . $caption . "</caption>\n";
//   }
//
//   // Format the table columns:
//   if (count($colgroups)) {
//     foreach ($colgroups as $number => $colgroup) {
//       $attributes = array();
//
//       // Check if we're dealing with a simple or complex column
//       if (isset($colgroup['data'])) {
//         foreach ($colgroup as $key => $value) {
//           if ($key == 'data') {
//             $cols = $value;
//           }
//           else {
//             $attributes[$key] = $value;
//           }
//         }
//       }
//       else {
//         $cols = $colgroup;
//       }
//
//       // Build colgroup
//       if (is_array($cols) && count($cols)) {
//         $output .= ' <colgroup' . drupal_attributes($attributes) . '>';
//         $i = 0;
//         foreach ($cols as $col) {
//           $output .= ' <col' . drupal_attributes($col) . ' />';
//         }
//         $output .= " </colgroup>\n";
//       }
//       else {
//         $output .= ' <colgroup' . drupal_attributes($attributes) . " />\n";
//       }
//     }
//   }
//
//   // Add the 'empty' row message if available.
//   if (!count($rows) && $empty) {
//     $header_count = 0;
//     foreach ($header as $header_cell) {
//       if (is_array($header_cell)) {
//         $header_count += isset($header_cell['colspan']) ? $header_cell['colspan'] : 1;
//       }
//       else {
//         $header_count++;
//       }
//     }
//     $rows[] = array(array('data' => $empty, 'colspan' => $header_count, 'class' => array('empty', 'message')));
//   }
//
//   // Format the table header:
//   if (count($header)) {
//     $ts = tablesort_init($header);
//     // HTML requires that the thead tag has tr tags in it followed by tbody
//     // tags. Using ternary operator to check and see if we have any rows.
//     $output .= (count($rows) ? ' <thead><tr>' : ' <tr>');
//     foreach ($header as $cell) {
//       $cell = tablesort_header($cell, $header, $ts);
//       $output .= _theme_table_cell($cell, TRUE);
//     }
//     // Using ternary operator to close the tags based on whether or not there are rows
//     $output .= (count($rows) ? " </tr></thead>\n" : "</tr>\n");
//   }
//   else {
//     $ts = array();
//   }
//
//   // Format the table rows:
//   if (count($rows)) {
//     $output .= "<tbody>\n";
//     $flip = array('even' => 'odd', 'odd' => 'even');
//     $class = 'even';
//     foreach ($rows as $number => $row) {
//       // Check if we're dealing with a simple or complex row
//       if (isset($row['data'])) {
//         $cells = $row['data'];
//         $no_striping = TRUE;
//
//         // Set the attributes array and exclude 'data' and 'no_striping'.
//         $attributes = $row;
//         unset($attributes['data']);
//         unset($attributes['no_striping']);
//       }
//       else {
//         $cells = $row;
//         $attributes = array();
//         $no_striping = TRUE;
//       }
//       if (count($cells)) {
//         // Add odd/even class
//         if (!$no_striping) {
//           $class = $flip[$class];
//           $attributes['class'][] = $class;
//         }
//
//         // Build row
//         $output .= ' <tr' . drupal_attributes($attributes) . '>';
//         $i = 0;
//         foreach ($cells as $cell) {
//           $cell = tablesort_cell($cell, $header, $ts, $i++);
//           $output .= _theme_table_cell($cell);
//         }
//         $output .= " </tr>\n";
//       }
//     }
//     $output .= "</tbody>\n";
//   }
//
//   $output .= "</table>\n";
//   return $output;
// }

/**
 * Returns HTML for a query pager.
 *
 * Menu callbacks that display paged query results should call theme('pager') to
 * retrieve a pager control so that users can view other results. Format a list
 * of nearby pages with additional query results.
 *
 * @param $variables
 *   An associative array containing:
 *   - tags: An array of labels for the controls in the pager.
 *   - element: An optional integer to distinguish between multiple pagers on
 *     one page.
 *   - parameters: An associative array of query string parameters to append to
 *     the pager links.
 *   - quantity: The number of pages in the list.
 *
 * @ingroup themeable
 */
// function dnccoreui_pager($variables) {
//   $variables['attributes'] = array(
//     'class' => array('page-link'),
//   );
//   $tags = $variables['tags'];
//   $element = $variables['element'];
//   $parameters = $variables['parameters'];
//   $quantity = $variables['quantity'];
//   global $pager_page_array, $pager_total;
//
//   // Calculate various markers within this pager piece:
//   // Middle is used to "center" pages around the current page.
//   $pager_middle = ceil($quantity / 2);
//   // current is the page we are currently paged to
//   $pager_current = $pager_page_array[$element] + 1;
//   // first is the first page listed by this pager piece (re quantity)
//   $pager_first = $pager_current - $pager_middle + 1;
//   // last is the last page listed by this pager piece (re quantity)
//   $pager_last = $pager_current + $quantity - $pager_middle;
//   // max is the maximum page number
//   $pager_max = $pager_total[$element];
//   // End of marker calculations.
//
//   // Prepare for generation loop.
//   $i = $pager_first;
//   if ($pager_last > $pager_max) {
//     // Adjust "center" if at end of query.
//     $i = $i + ($pager_max - $pager_last);
//     $pager_last = $pager_max;
//   }
//   if ($i <= 0) {
//     // Adjust "center" if at start of query.
//     $pager_last = $pager_last + (1 - $i);
//     $i = 1;
//   }
//   // End of generation loop preparation.
//
//   $li_first = theme('pager_first', array('text' => (isset($tags[0]) ? $tags[0] : t('« first')), 'element' => $element, 'parameters' => $parameters));
//   $li_previous = theme('pager_previous', array('text' => (isset($tags[1]) ? $tags[1] : t('‹ previous')), 'element' => $element, 'interval' => 1, 'parameters' => $parameters));
//   $li_next = theme('pager_next', array('text' => (isset($tags[3]) ? $tags[3] : t('next ›')), 'element' => $element, 'interval' => 1, 'parameters' => $parameters));
//   $li_last = theme('pager_last', array('text' => (isset($tags[4]) ? $tags[4] : t('last »')), 'element' => $element, 'parameters' => $parameters));
//
//   if ($pager_total[$element] > 1) {
//     if ($li_first) {
//       $items[] = array(
//         'class' => array('page-item'),
//         'data' => $li_first,
//       );
//     }
//     if ($li_previous) {
//       $items[] = array(
//         'class' => array('page-item'),
//         'data' => $li_previous,
//       );
//     }
//
//     // When there is more than one page, create the pager list.
//     if ($i != $pager_max) {
//       if ($i > 1) {
//         $items[] = array(
//           'class' => array('pager-ellipsis'),
//           'data' => '…',
//         );
//       }
//       // Now generate the actual pager piece.
//       for (; $i <= $pager_last && $i <= $pager_max; $i++) {
//         if ($i < $pager_current) {
//           $items[] = array(
//             'class' => array('page-item'),
//             'data' => theme('pager_previous', array('text' => $i, 'element' => $element, 'interval' => ($pager_current - $i), 'parameters' => $parameters)),
//           );
//         }
//         if ($i == $pager_current) {
//           $items[] = array(
//             'class' => array('page-item', 'active'),
//             'data' => '<a class="page-link" href="#">' . $i . '</a>',
//           );
//         }
//         if ($i > $pager_current) {
//           $items[] = array(
//             'class' => array('page-item'),
//             'data' => theme('pager_next', array('text' => $i, 'element' => $element, 'interval' => ($i - $pager_current), 'parameters' => $parameters)),
//           );
//         }
//       }
//       if ($i < $pager_max) {
//         $items[] = array(
//           'class' => array('pager-ellipsis'),
//           'data' => '…',
//         );
//       }
//     }
//     // End generation.
//     if ($li_next) {
//       $items[] = array(
//         'class' => array('page-item'),
//         'data' => $li_next,
//       );
//     }
//     if ($li_last) {
//       $items[] = array(
//         'class' => array('page-item'),
//         'data' => $li_last,
//       );
//     }
//     return '<h2 class="element-invisible">' . t('Pages') . '</h2>' . theme('item_list', array(
//       'items' => $items,
//       'attributes' => array('class' => array('pagination')),
//     ));
//   }
// }

/**
 * Returns HTML for a list or nested list of items.
 *
 * @param $variables
 *   An associative array containing:
 *   - items: An array of items to be displayed in the list. If an item is a
 *     string, then it is used as is. If an item is an array, then the "data"
 *     element of the array is used as the contents of the list item. If an item
 *     is an array with a "children" element, those children are displayed in a
 *     nested list. All other elements are treated as attributes of the list
 *     item element.
 *   - title: The title of the list.
 *   - type: The type of list to return (e.g. "ul", "ol").
 *   - attributes: The attributes applied to the list element.
 */
// function dnccoreui_item_list($variables) {
//   $items = $variables['items'];
//   $title = $variables['title'];
//   $type = $variables['type'];
//   $attributes = $variables['attributes'];
//
//   // Only output the list container and title, if there are any list items.
//   // Check to see whether the block title exists before adding a header.
//   // Empty headers are not semantic and present accessibility challenges.
//   if (!empty($attributes['class']) && in_array('pagination', $attributes['class'])) {
//     $output =  '<nav>';
//     $nav = TRUE;
//   }
//   else {
//     $output =  '<div class="item-list">';
//   }
//
//   if (isset($title) && $title !== '') {
//     $output .= '<h3>' . $title . '</h3>';
//   }
//
//   if (!empty($items)) {
//     $output .= "<$type" . drupal_attributes($attributes) . '>';
//     $num_items = count($items);
//     $i = 0;
//     foreach ($items as $item) {
//       $attributes = array();
//       $children = array();
//       $data = '';
//       $i++;
//       if (is_array($item)) {
//         foreach ($item as $key => $value) {
//           if ($key == 'data') {
//             $data = $value;
//           }
//           elseif ($key == 'children') {
//             $children = $value;
//           }
//           else {
//             $attributes[$key] = $value;
//           }
//         }
//       }
//       else {
//         $data = $item;
//       }
//       if (count($children) > 0) {
//         // Render nested list.
//         $data .= theme_item_list(array('items' => $children, 'title' => NULL, 'type' => $type, 'attributes' => $attributes));
//       }
//       if ($i == 1) {
//         $attributes['class'][] = 'first';
//       }
//       if ($i == $num_items) {
//         $attributes['class'][] = 'last';
//       }
//       $output .= '<li' . drupal_attributes($attributes) . '>' . (is_array($data) ? drupal_render($data) : $data) . "</li>\n";
//     }
//     $output .= "</$type>";
//   }
//   $output .= !empty($nav) ? '</nav>' : '</div>';
//   unset ($nav);
//   return $output;
// }
