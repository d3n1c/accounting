<?php
/**
 * @file
 * Enables modules and site configuration for a standard site installation.
 */

/**
 * Implements hook_form_FORM_ID_alter() for install_configure_form().
 *
 * Allows the profile to alter the site configuration form.
 */
function accounting_form_install_configure_form_alter(&$form, &$form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
}

function accounting_install_tasks_alter(&$tasks) {
  unset ($tasks['install_configure_form']);
  $target_theme = 'seven';
  if ($GLOBALS['theme'] != $target_theme) {
    unset($GLOBALS['theme']);

    drupal_static_reset();
    $GLOBALS['conf']['maintenance_theme'] = $target_theme;
    _drupal_maintenance_theme();
  }
}

function accounting_nomenclatures() {
  return [
    'site_name' => 'Accounting for Point of Sales',
    'site_mail' => 'site@examle.org',
    'date_default_time_zone' => 'Asia/Makassar',
    'site_default_country' => 'ID',
    'clean_url' => 0,
    'install_time' => $_SERVER['REQUEST_TIME'],
  ];
}

function accounting_resources() {
  return [
    [
      'name' => 'dncaccountqueue',
      'region' => 'navigation'
    ],
    [
      'name' => 'dncaccountqueueaction',
      'region' => 'sidebar_second',
      'weight' => 0,
    ],
    [
      'name' => 'dncaccountingbalances',
      'region' => 'sidebar_second',
      'weight' => 1
    ]
  ];
}

function accounting_prepare_main_menus() {
  return [
    'parent' => [
      'reference' => [
        'link_title' => st('Reference'),
        'link_path' => '<front>',
        'menu_name' => 'main-menu',
        'expanded' => TRUE,
        'weight' => 1
      ],
      'administration' => [
        'link_title' => st('Administration'),
        'link_path' => '<front>',
        'menu_name' => 'main-menu',
        'expanded' => TRUE,
        'weight' => 2
      ],
      'transaction' => [
        'link_title' => st('Transaction'),
        'link_path' => '<front>',
        'menu_name' => 'main-menu',
        'expanded' => TRUE,
        'weight' => 3
      ],
      'report' => [
        'link_title' => st('Report'),
        'link_path' => '<front>',
        'menu_name' => 'main-menu',
        'expanded' => TRUE,
        'weight' => 4
      ]
    ],
    'children' => [
      'account' => [
        'link_title' => st('Account'),
        'link_path' => 'dncaccounting/references/accounts',
        'menu_name' => 'main-menu',
        'weight' => 11,
        'parent' => 'reference'
      ],
      'category' => [
        'link_title' => st('Category'),
        'link_path' => 'dncaccounting/references/categories',
        'menu_name' => 'main-menu',
        'weight' => 12,
        'parent' => 'reference'
      ],
      'commodity' => [
        'link_title' => st('Commodity'),
        'link_path' => 'dncaccounting/references/commodities',
        'menu_name' => 'main-menu',
        'weight' => 13,
        'parent' => 'reference'
      ],
      'sellingprice' => [
        'link_title' => st('Selling Price'),
        'link_path' => 'dncaccounting/set-price',
        'menu_name' => 'main-menu',
        'weight' => 21,
        'parent' => 'administration'
      ],
      'generalt' => [
        'link_title' => st('General'),
        'link_path' => 'dncaccounting/transaction',
        'menu_name' => 'main-menu',
        'weight' => 31,
        'parent' => 'transaction'
      ],
      'purchase' => [
        'link_title' => st('Purchase'),
        'link_path' => 'dncaccounting/purchase-transaction',
        'menu_name' => 'main-menu',
        'weight' => 32,
        'parent' => 'transaction'
      ],
      'sale' => [
        'link_title' => st('Sale'),
        'link_path' => 'dncaccounting/sale-transaction',
        'menu_name' => 'main-menu',
        'weight' => 33,
        'parent' => 'transaction'
      ],
      'journal' => [
        'link_title' => st('Journal'),
        'link_path' => 'dncaccounting/report/journals',
        'menu_name' => 'main-menu',
        'weight' => 41,
        'parent' => 'report'
      ],
      'profitlost' => [
        'link_title' => st('Profit or Lost'),
        'link_path' => 'dncaccounting/report/profit-lost',
        'menu_name' => 'main-menu',
        'weight' => 42,
        'parent' => 'report'
      ],
      'balancesheet' => [
        'link_title' => st('Balance Sheet'),
        'link_path' => 'dncaccounting/report/balance-sheet',
        'menu_name' => 'main-menu',
        'weight' => 43,
        'parent' => 'report'
      ],
      'saldo' => [
        'link_title' => st('Saldo'),
        'link_path' => 'dncaccounting/report/balances',
        'menu_name' => 'main-menu',
        'weight' => 44,
        'parent' => 'report'
      ],
      'stock' => [
        'link_title' => st('Stock'),
        'link_path' => 'dncaccounting/report/stocks',
        'menu_name' => 'main-menu',
        'weight' => 45,
        'parent' => 'report'
      ]
    ]
  ];
}