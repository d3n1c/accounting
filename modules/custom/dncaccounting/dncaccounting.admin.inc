<?php

/**
 * @file
 * Handling administration action in the system
 *
 * All configuration processing will be handled
 * by this module
 */

function dncaccounting_admin_menu(&$items) {
  $items['admin/config/dncsystem/dncaccounting'] = array(
    'title' => 'Accounting',
    'description' => 'Settings for DNC\'s System to handling accounting',
    'position' => 'right',
    'weight' => 0,
    'page callback' => 'system_admin_menu_block_page',
    'access arguments' => array('administer site configuration'),
    'file' => 'system.admin.inc',
    'file path' => drupal_get_path('module', 'system'),
    );
  $items['admin/config/dncsystem/dncaccounting/global'] = array(
    'title' => 'Global Params',
    'description' => 'Stock management parameters and configurations',
    'access arguments' => array('administer site configuration'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('dncaccounting_admin_params'),
    'file' => 'dncaccounting.admin.inc',
  );
  $items['admin/structure/dncsystem/dncaccounting'] = array(
    'title' => 'Accounting',
    'description' => 'Some Settings for DNC\'s System to handling accounting',
    'position' => 'right',
    'weight' => 0,
    'page callback' => 'system_admin_menu_block_page',
    'access arguments' => array('administer site configuration'),
    'file' => 'system.admin.inc',
    'file path' => drupal_get_path('module', 'system'),
  );
  $items['admin/structure/dncsystem/dncaccounting/scenarios'] = array(
    'title' => 'Scenarios',
    'description' => 'Accounting journals posting scenarios',
    'access arguments' => array('administer dnc accounting'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('dncaccounting_admin_scenarios'),
    'file' => 'dncaccounting.admin.inc',
  );
  $items['admin/structure/dncsystem/dncaccounting/scenarios/add'] = array(
    'title' => 'Add Scenarios',
    'description' => 'Adding accounting journals posting scenario',
    'access arguments' => array('administer dnc accounting'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('dncaccounting_admin_scenario_add'),
    'file' => 'dncaccounting.admin.inc',
    'type' => MENU_LOCAL_ACTION
  );
  $items['admin/structure/dncsystem/dncaccounting/scenarios/%dncaccounting'] = array(
    'title' => 'Add Scenarios',
    'description' => 'Adding accounting journals posting scenario',
    'access arguments' => array('administer dnc accounting'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('dncaccounting_admin_scenario_edit', 5),
    'file' => 'dncaccounting.admin.inc',
    'type' => MENU_VISIBLE_IN_BREADCRUMB
  );
  $items['admin/structure/dncsystem/dncaccounting/report-types'] = array(
    'title' => 'Report Types',
    'description' => 'Set accounts to type of report',
    'access arguments' => array('administer dnc accounting'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('dncaccounting_admin_report_type'),
    'file' => 'dncaccounting.admin.inc',
  );
  $items['dncaccounting/set-price'] = [
    'title' => 'Pricing',
    'description' => 'Set price of commodities',
    'access arguments' => ['administer dnc accounting'],
    'page callback' => 'drupal_get_form',
    'page arguments' => ['dncaccounting_admin_set_price'],
    'file' => 'dncaccounting.admin.inc',
    'type' => MENU_CALLBACK,
  ];
}

/**
 * Configure heap of assets parameters
 *
 * @param array $form
 *   variable array of form
 * @param array $form_state
 *   variable array of form_state
 *
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dncaccounting_admin_params(array $form = array(), array $form_state = array()) {
//  $dump = dncsessclient_get_credentials();
//  if (empty($dump)) {
//    unset ($dump);
//    drupal_set_message(t('Set at least one credential to connect the system with point of sale applications'));
//    drupal_goto('admin/structure/dncsystem/dncsessclient');
//    return $form;
//  }
//  $agents = [];
//  foreach ($dump as $keys => $values) {
//    $agents[$values['nid']] = $values['detail']['baseurl'];
//  }
//  unset ($dump);
  
  $default = variable_get('dncaccounting_configs', []);
//  echo '<pre>'; print_r($default); echo '</pre>'; exit;
  $form['info'] = [
    '#type' => 'item',
    '#weight' => 5,
    '#markup' => t('It\'s parameter just for the client of drupal services. Not working for another framework'),
  ];
//  $form['pointofsale'] = [
//    '#type' => 'fieldset',
//    '#title' => t('Point of Sale Configuration Parameters'),
//    '#weight' => 10,
//    '#collapsible' => TRUE,
//    '#collapsed' => empty($default) ? FALSE : TRUE,
//  ];
//  $form['pointofsale']['dnc_configs[pos_agent]'] = [
//    '#type' => 'select',
//    '#weight' => 20,
//    '#title' => t('POS Service Agent') . ' *',
//    '#description' => t('URL of the agent to make connection with point of sales application'),
//    '#options' => $agents,
//    '#default_value' => empty($default['pos_agent']) ? current(array_keys($agents)) : $default['pos_agent'],
//  ];
  $form['stock'] = [
    '#type' => 'fieldset',
    '#title' => t('Stock Configuration Parameters'),
    '#weight' => 20,
    '#collapsible' => TRUE,
    '#collapsed' => empty($default) ? FALSE : TRUE,
  ];
  $form['stock']['accounts'] = [
    '#type' => 'fieldset',
    '#title' => t('Fixed Accounts Configurations'),
    '#weight' => 10,
    '#collapsible' => TRUE,
  ];
  $setaccounts = dncaccounting_get_fixed_account_names();
  foreach ($setaccounts as $keys => $values) {
    $vtitle = str_replace('_', ' ', str_replace('-', ' ', $values));
    $form['stock']['accounts']['dnc_configs[accounts][' . $values . ']'] = [
      '#type' => 'textfield',
      '#weight' => 10 + $keys,
      '#title' => t('@acnt Account', array('@acnt' => ucwords(strtolower($vtitle)))) . ' *',
      '#description' => t('Account that represented to @title', array('@title' => $vtitle)),
      '#autocomplete_path' => 'dncaccounting/search-autocomplete/accounts',
    ];
    unset ($vtitle);
    if (!empty($default['accounts'][$values])) {
      $account = dncaccounting_get_data('accounts', $default['accounts'][$values]);
      if (!empty($account['nid'])) {
        $account = $account['title'] . '; ' . $account['description'] . ' | ' . $account['nid'];
        $form['stock']['accounts']['dnc_configs[accounts][' . $values . ']']['#default_value'] = $account;
      }
      unset ($account);
    }
  }
  unset ($setaccounts);
//  $form['stock']['dnc_configs[stock_agent]'] = [
//    '#type' => 'select',
//    '#weight' => 20,
//    '#title' => t('Stock Service Agent') . ' *',
//    '#description' => t('URL of the agent to make connection with stock management'),
//    '#options' => $agents,
//    '#default_value' => empty($default['stock_agent']) ? current(array_keys($agents)) : $default['stock_agent'],
//  ];
//  unset ($agents);
  $form['stock']['dnc_configs[stock_sale_flow]'] = [
    '#type' => 'select',
    '#weight' => 20,
    '#title' => t('Stock Sale Flow') . ' *',
    '#description' => t('Flow method of sales for stock management'),
    '#options' => [
      0 => t('FIFO (First in first out)'),
      1 => t('LIFO (Last in first out)')
    ],
    '#default_value' => empty($default['stock_sale_flow']) ? 0 : $default['stock_sale_flow'],
  ];
  $form['stock']['dnc_configs[show_addpay]'] = [
    '#type' => 'checkbox',
    '#weight' => 30,
    '#title' => t('Show Add Pay'),
    '#description' => t('Showing add pay element in selling transaction form'),
    '#default_value' => empty($default['show_addpay']) ? false : true,
  ];
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('save'),
    '#weight' => 500,
  );
  return $form;
}

function dncaccounting_admin_params_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);

  $form_state['redirect'] = current_path();
  if (!empty($form_state['input']['dnc_configs'])) {
    if (!empty($form_state['input']['dnc_configs']['accounts'])) {
      $setaccounts = dncaccounting_get_fixed_account_names();
      $dump = [];
      foreach ($form_state['input']['dnc_configs']['accounts'] as $keys => $values) {
        if (!in_array($keys, $setaccounts)) {
          continue;
        }
        $account = $values;
        $account = explode('|', $account);
        if (!empty($account[count($account) - 1])) {
          $account = trim($account[count($account) - 1]);
          settype($account, 'int');
          $dump[$keys] = $account;
        }
        unset ($account);
      }
      unset($setaccounts);
      $form_state['input']['dnc_configs']['accounts'] = $dump;
      unset ($dump);
    }
    variable_set('dncaccounting_configs', $form_state['input']['dnc_configs']);
  }
  drupal_set_message(t('configurations have been successfully saved :)'));
}

function dncaccounting_admin_get_cookies() {
  $result = [
    'stock' => dncaccounting_get_cookie('stock'),
    'pos' => dncaccounting_get_cookie('pos')
  ];
  drupal_json_output($result);
}

function dncaccounting_admin_firstly_scenarios() {
  $gconfigs = variable_get('dncaccounting_configs', []);
  if (empty($gconfigs['accounts']['cash'])
      || empty($gconfigs['accounts']['inventories'])
      || empty($gconfigs['accounts']['debt'])
      ) {
    unset ($gconfigs);
    return;
  }
  $scenarios = [];
  $scenarios[1] = [
    'title' => t('Purchasing goods inventory IN CASH'),
    'detail' => [
      [
        'account' => $gconfigs['accounts']['inventories'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['cash'],
        'action' => 1
      ],
    ]
  ];
  $scenarios[2] = [
    'title' => t('Purchasing goods inventory NON CASH'),
    'detail' => [
      [
        'account' => $gconfigs['accounts']['inventories'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['debt'],
        'action' => 1
      ],
    ]
  ];
  $scenarios[3] = [
    'title' => t('Purchasing goods inventory IN PARTIAL CASH'),
    'detail' => [
      [
        'account' => $gconfigs['accounts']['inventories'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['cash'],
        'action' => 1
      ],
      [
        'account' => $gconfigs['accounts']['debt'],
        'action' => 1
      ],
    ]
  ];
  $scenarios[4] = [
    'title' => t('Sale goods IN CASH'),
    'detail' => [
      [
        'account' => $gconfigs['accounts']['cash'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['sale'],
        'action' => 1
      ],
      [
        'account' => $gconfigs['accounts']['cost_of_good_sold'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['inventories'],
        'action' => 1
      ],
    ]
  ];
  $scenarios[5] = [
    'title' => t('Sale goods NON CASH'),
    'detail' => [
      [
        'account' => $gconfigs['accounts']['account_receivable'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['sale'],
        'action' => 1
      ],
      [
        'account' => $gconfigs['accounts']['cost_of_good_sold'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['inventories'],
        'action' => 1
      ],
    ]
  ];
  $scenarios[6] = [
    'title' => t('Sale goods IN PARTIAL CASH'),
    'detail' => [
      [
        'account' => $gconfigs['accounts']['account_receivable'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['cash'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['sale'],
        'action' => 1
      ],
      [
        'account' => $gconfigs['accounts']['cost_of_good_sold'],
        'action' => 0
      ],
      [
        'account' => $gconfigs['accounts']['inventories'],
        'action' => 1
      ],
    ]
  ];
  unset ($gconfigs);
  foreach ($scenarios as $keys => $values) {
    dncaccounting_save_updated_data('scenarios', $values);
  }
  return $scenarios;
}

function dncaccounting_admin_scenarios(array $form = [], array $form_state = []) {
  module_load_include('inc', 'dncaccounting', 'dncaccounting.reference');
  $scenarios = dncaccounting_references_get_data('scenarios');
  if (empty($scenarios)) {
    $scenarios = dncaccounting_admin_firstly_scenarios();
  }
//  echo '<pre>'; print_r($scenarios); echo '</pre>'; exit;
  if (empty($scenarios)) {
    unset ($scenarios);
    drupal_set_message(t('Not found any scenarios'), 'error', TRUE);
    return $form;
  }
  
  krsort($scenarios);
  
  $form['item'] = [
    '#type' => 'tableselect',
    '#weight' => 10,
    '#options' => [],
    '#empty' => t('No data available yet'),
    '#header' => [
      'title' => ['data' => t('Title')],
      'journal' => ['data' => t('Journals')],
    ],
  ];
  
  module_load_include('inc', 'dncsystem', 'dncsystem.array');
  $scenarios = dncsystem_array_get_data_chunck($scenarios, 10);
  
  $count = 0;
  foreach ($scenarios as $keys => $values) {
    $journals = [];
    foreach ($values['detail'] as $key => $value) {
      $account = dncaccounting_get_data('accounts', $value['account']);
      if (!empty($account['title'])) {
        $journals[] = (!empty($value['action']) ? ' ' . str_repeat('&nbsp;', 4) . ' ' : NULL) . $account['title'] . ' [' . (empty($value['action']) ? t('debit') : t('credit')) . ']';
      }
      unset ($account);
    }
    $journals = implode("\n" . '<br />', $journals);
    $form['item']['#options'][$keys]['title']['data'] = l($values['title'], 'admin/structure/dncsystem/dncaccounting/scenarios/' . $keys, [
      'html' => TRUE
    ]);
    $form['item']['#options'][$keys]['journal']['data'] = $journals;
    unset ($journals);
    if ($keys < 7) {
      $form['item'][$keys]['#disabled'] = TRUE;
    }
    else {
      $count++;
    }
  }
  unset ($scenarios);
  
  if (!empty($count)) {
    $form['submit'] = [
      '#type' => 'submit',
      '#weight' => 100,
      '#value' => t('Delete Choosed'),
      '#attributes' => ['class' => ['btn', 'btn-danger']],
    ];
  }
  
  $form['pager'] = [
    '#type' => 'item',
    '#weight' => 200,
    '#markup' => theme('pager', ['quantity' => 10])
  ];
  
  return $form;
}

function dncaccounting_admin_scenarios_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  if (!empty($form_state['input']['item'])) {
    foreach ($form_state['input']['item'] as $keys => $values) {
      if (!empty($values)) {
        dncaccounting_delete_data('scenarios', $values);
      }
    }
  }
  drupal_set_message(t('The choosed items have been remmove successfully'));
}

function dncaccounting_admin_scenario_add(array $form = [], array $form_state = []) {
  return dncaccounting_admin_scenario_update($form, $form_state);
}

function dncaccounting_admin_scenario_edit(array $form = [], array $form_state = [], $nid) {
  $form_state['storage']['nid'] = $nid;
  return dncaccounting_admin_scenario_update($form, $form_state);
}

function dncaccounting_admin_scenario_update(array $form = [], array $form_state = []) {
  if (!empty($form_state['storage']['default'])) {
    if (!empty($form_state['storage']['need_to_confirmed'])) {
      foreach ($form_state['storage'] as $keys => $values) {
        $form[$keys] = [
          '#type' => 'value',
          '#value' => $values
        ];
      }
      $question = t('Are you sure to save this scenario') . ' ?';
      $path = 'admin/structure/dncsystem/dncaccounting';
      $yes = t('Yes Save Now');
      $no = t('Cancel');
      $description = [];
      $description[] = '<strong>' . t('Title') . ':</strong> ' . $form_state['storage']['default']['title'];
      $description[] = '<br /><strong>' . t('Journal scenario') . ':</strong> ';
      $table = [
        'rows' => [],
        'header' => [
          'debit' => ['data' => t('Debit')],
          'credit' => ['data' => t('Credit')]
        ]
      ];
      foreach ($form_state['storage']['default']['detail'] as $keys => $values) {
        $account = dncaccounting_get_data('accounts', $values['account']);
        $table['rows'][$keys]['debit']['data'] = empty($values['action']) ? $account['title'] : NULL;
        $table['rows'][$keys]['credit']['data'] = empty($values['action']) ? NULL : $account['title'];
        unset ($account);
      }
      $description[] = '<br />' . theme('table', $table);
      unset ($table);
      $description[] = '<br /><br />';
      $description = implode("\n", $description);
      return confirm_form($form, $question, $path, $description, $yes, $no);
    }
    $default = $form_state['storage']['default'];
    $form['nid'] = [
      '#type' => 'value',
      '#value' => $form_state['storage']['nid']
    ];
  }
  else {
    if (!empty($form_state['storage']['nid'])) {
      $form['nid'] = [
        '#type' => 'value',
        '#value' => $form_state['storage']['nid']
      ];
      $default = dncaccounting_get_data('scenarios', $form_state['storage']['nid']);
      if (empty($default['title'])) {
        unset ($default);
        drupal_set_message(t('No scenario found by the ID'), 'error', FALSE);
        return $form;
      }
    }
  }
  
  $form['title'] = [
    '#type' => 'textfield',
    '#weight' => 10,
    '#title' => t('Title'),
    '#default_value' => empty($default['title']) ? NULL : $default['title']
  ];
  
  $form['fdetail'] = [
    '#type' => 'fieldset',
    '#weight' => 20,
    '#title' => t('Journal Scenarios'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  ];
  
  $form['fdetail']['add[account]'] = [
    '#type' => 'textfield',
    '#weight' => 10,
    '#autocomplete_path' => 'dncaccounting/search-autocomplete/accounts',
    '#attributes' => ['placeholder' => t('Add Account [required]')],
    '#prefix' => '<div class="container-inline">',
    '#size' => 60
  ];
  $form['fdetail']['add[action]'] = [
    '#type' => 'select',
    '#weight' => 11,
    '#options' => [t('debit'), t('credit')],
  ];
  $form['fdetail']['additem'] = [
    '#type' => 'submit',
    '#weight' => 12,
    '#value' => t('Add'),
    '#suffix' => '</div>',
    '#attributes' => ['class' => ['btn', 'btn-success']],
    '#submit' => ['dncaccounting_admin_scenario_update_additem']
  ];
  
  $form['fdetail']['itable'] = [
    '#theme' => 'table',
    '#weight' => 20,
    '#rows' => [],
    '#header' => [
      'account' => ['data' => t('Account')],
      'action' => ['data' => t('Action')],
      'delete' => [
        'data' => t('Delete'),
        'style' => 'text-align: center',
        'width' => '5%'
      ]
    ],
    '#prefix' => '<div class="col col-sm-12 col-md-12 col-lg-12">',
    '#suffix' => '</div>',
  ];
  
  if (!empty($default['detail'])) {
    foreach ($default['detail'] as $keys => $values) {
      $reference = dncaccounting_get_data('accounts', $values['account']);
      $account = [
        '#id' => 'detail-' . $keys . '-account',
        '#type' => 'hidden',
        '#name' => 'detail[' . $keys . '][account]',
        '#value' => $values['account']
      ];
      $vaccount = [
        '#id' => 'detail-' . $keys . '-account',
        '#type' => 'item',
        '#markup' => $reference['title']
      ];
      $action = [
        '#id' => 'detail-' . $keys . '-action',
        '#type' => 'select',
        '#name' => 'detail[' . $keys . '][action]',
        '#options' => [t('debit'), t('credit')],
        '#value' => $values['action']
      ];
      $delete = [
        '#id' => 'detail-' . $keys . '-delete',
        '#type' => 'checkbox',
        '#name' => 'detail[' . $keys . '][delete]',
      ];
      if (!empty($values['delete'])) {
        $delete['#value'] = TRUE;
      }
      $form['fdetail']['itable']['#rows'][$keys]['account']['data'] = [&$account, &$vaccount];
      $form['fdetail']['itable']['#rows'][$keys]['action']['data'] = &$action;
      $form['fdetail']['itable']['#rows'][$keys]['delete']['data'] = &$delete;
      $form['fdetail']['itable']['#rows'][$keys]['delete']['style'] = 'text-align: center';
      unset ($reference, $account, $action, $vaccount, $delete);
    }
  }
  
  $form['fdetail']['edititem'] = [
    '#type' => 'submit',
    '#weight' => $form['fdetail']['itable']['#weight'] + 1,
    '#value' => t('Edit'),
    '#attributes' => ['class' => ['btn', 'btn-success']],
    '#submit' => ['dncaccounting_admin_scenario_update_edititem'],
    '#prefix' => '<div style="text-align: right" align="right">',
    '#suffix' => '</div>'
  ];
  
  $form['submit'] = [
    '#type' => 'submit',
    '#weight' => 100,
    '#value' => t('Save'),
    '#attributes' => ['class' => ['btn', 'btn-success']],
    '#prefix' => '<div class="container-inline">',
  ];
  $form['reset'] = [
    '#type' => 'submit',
    '#weight' => $form['submit']['#weight'] + 1,
    '#value' => t('Reset'),
    '#attributes' => ['class' => ['btn', 'btn-warning']],
    '#suffix' => '</div>',
    '#submit' => ['dncaccounting_admin_scenario_update_reset']
  ];
  return $form;
}

function dncaccounting_admin_scenario_update_collect(&$form_state) {
  if (!empty($form_state['values']['nid'])) {
    $form_state['storage']['nid'] = $form_state['values']['nid'];
  }
  $form_state['storage']['default']['title'] = $form_state['input']['title'];
  $form_state['storage']['default']['detail'] = $form_state['input']['detail'];
}

function dncaccounting_admin_scenario_update_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  if (empty($form_state['values']['confirm'])) {
    $form_state['storage']['need_to_confirmed'] = TRUE;
    $form_state['rebuild'] = TRUE;
    dncaccounting_admin_scenario_update_collect($form_state);
    if (!empty($form_state['storage']['default']['detail'])) {
      $dump = [];
      foreach ($form_state['storage']['default']['detail'] as $keys => $values) {
        $dump[] = $values;
      }
      $form_state['storage']['default']['detail'] = $dump;
      unset ($dump);
    }
    return;
  }
  $form_state['redirect'] = empty($_GET['destination']) ? 'admin/structure/dncsystem/dncaccounting/scenarios' : $_GET['destination'];
  $data = [];
  $data['title'] = $form_state['values']['default']['title'];
  $data['detail'] = $form_state['values']['default']['detail'];
  if (!empty($form_state['values']['nid'])) {
    $data['nid'] = $form_state['values']['nid'];
  }
  dncaccounting_save_updated_data('scenarios', $data);
  drupal_set_message(t('The scenario has been saved'));
}

function dncaccounting_admin_scenario_add_submit(&$form, &$form_state) {
  dncaccounting_admin_scenario_update_submit($form, $form_state);
}

function dncaccounting_admin_scenario_edit_submit(&$form, &$form_state) {
  dncaccounting_admin_scenario_update_submit($form, $form_state);
}

function dncaccounting_admin_scenario_update_additem(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['rebuild'] = TRUE;
  dncaccounting_admin_scenario_update_collect($form_state);
  foreach ($form_state['storage']['default']['detail'] as $keys => $values) {
    if (!empty($values['delete'])) {
      unset ($form_state['storage']['default']['detail'][$keys]['delete']);
    }
  }
  if (empty($form_state['input']['add']['account'])) {
    form_set_error('add[account]', t('Account is required'));
    return;
  }
  $account = explode('|', $form_state['input']['add']['account']);
  $account = empty($account[count($account) - 1]) ? NULL : trim($account[count($account) - 1]);
  $account = !is_numeric($account) ? NULL : $account;
  if (empty($account)) {
    unset ($account);
    form_set_error('add[account]', t('No Account found by your request'));
    return;
  }
  settype($account, 'int');
  $form_state['storage']['default']['detail'][] = [
    'account' => $account,
    'action' => $form_state['input']['add']['action']
  ];
  unset($account);
}

function dncaccounting_admin_scenario_update_edititem(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['rebuild'] = TRUE;
  dncaccounting_admin_scenario_update_collect($form_state);
  foreach ($form_state['storage']['default']['detail'] as $keys => $values) {
    if (!empty($values['delete'])) {
      unset ($form_state['storage']['default']['detail'][$keys]);
    }
  }
}

function dncaccounting_admin_scenario_update_reset(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
}

function dncaccounting_admin_set_price(array $form = [], array $form_state = []) {
  $max = dncstorage_get_max_id('commodities');
  if (empty($max)) {
    unset ($max);
    drupal_set_message(t('No data found yet'), 'error', FALSE);
    return $form;
  }
  
  $form['itable'] = [
    '#theme' => 'table',
    '#rows' => [],
    '#weight' => 10,
    '#header' => [
      'commodity' => ['data' => t('Commodity')],
      'unit' => ['data' => t('Unit')],
      'cost' => [
        'data' => t('Purchase Cost'),
        'style' => 'text-align: right'
      ],
      'price' => [
        'data' => t('Selling Price'),
        'style' => 'text-align: right'
      ]
    ],
  ];
  
  $limit = 10;
  $current = pager_default_initialize($max, $limit);
  unset ($max);
  $current = $current * $limit;
  
  $data = dncstorage_get_data('commodities', NULL, [], FALSE, [$current, $limit]);
  $getcount = 0;
  module_load_include('inc', 'dncaccounting', 'dncaccounting.stock');
  foreach ($data as $keys => $values) {
    $commodity = [
      '#id' => 'prices-' . $keys . '-commodity',
      '#type' => 'hidden',
      '#name' => 'prices[' . $keys . '][commodity]',
      '#value' => $values['nid'],
    ];
    $vcommodity = [
      '#id' => 'prices-' . $keys . '-vcommodity',
      '#type' => 'item',
      '#markup' => $values['title'] . (empty($values['description']) ? NULL : '<br /><small>' . $values['description'] . '</small>'),
    ];
    $unit = [
      '#id' => 'prices-' . $keys . '-unit',
      '#type' => 'item',
      '#markup' => $values['unit'],
    ];
    $cost = [
      '#id' => 'prices-' . $keys . '-cost',
      '#type' => 'item',
      '#markup' => NULL,
    ];
    $price = [
      '#id' => 'prices-' . $keys . '-price',
      '#type' => 'textfield',
      '#size' => 10,
      '#name' => 'prices[' . $keys . '][price]',
      '#value' => empty($values['price']) ? NULL : $values['price'],
      '#attributes' => ['style' => 'text-align: right']
    ];
    $stock = dncaccounting_stock_get_data($values['nid'], TRUE);
    if (empty($stock['stocks'])) {
      unset ($stock);
      $price['#type'] = 'item';
      $price['#markup'] = empty($price['#value']) ? NULL : $price['#value'];
      unset ($price['#name'], $price['#value']);
    }
    else {
      $getcount++;
      $ecost = 0;
      foreach ($stock['stocks'] as $value) {
        if (!empty($value['quantity']) && !empty($value['cost']) && $value['cost'] > $ecost) {
          $ecost = $value['cost'];
        }
      }
      $cost['#markup'] = number_format($ecost, 0, ',', '.');
      unset ($ecost);
    }
    $form['itable']['#rows'][$keys] = [
      'commodity' => ['data' => [&$commodity, &$vcommodity]],
      'unit' => ['data' => &$unit],
      'cost' => [
        'data' => &$cost,
        'style' => 'text-align: right',
      ],
      'price' => [
        'data' => &$price,
        'style' => 'text-align: right',
      ]
    ];
    unset ($stock, $commodity, $vcommodity, $unit, $cost, $price);
  }
  unset ($data);
  
  if (!empty($getcount)) {
    $form['submit'] = [
      '#type' => 'submit',
      '#weight' => 100,
      '#value' => t('Save'),
      '#attributes' => ['class' => ['btn', 'btn-success']],
      '#prefix' => '<div align="right" style="text-align: right" class="col-2 pull-right">',
      '#suffix' => '</div>'
    ];
  }
  else {
    drupal_set_message(t('No stock available yet'), 'warning', FALSE);
  }
  unset ($getcount);
  
  $form['pager'] = [
    '#type' => 'item',
    '#weight' => 200,
    '#markup' => theme('pager', ['quantity' => 10]),
  ];
  
  return $form;
}

function dncaccounting_admin_set_price_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  drupal_set_message(t('Prices have been saved'));
  
  foreach ($form_state['input']['prices'] as $keys => $values) {
    settype($values['price'], 'int');
    if (empty($values['price'])) {
      continue;
    }
    $commodity = dncaccounting_get_data('commodities', $values['commodity']);
    if (!empty($commodity['stocks'])) {
      $commodity['price'] = $values['price'];
      dncaccounting_save_updated_data('commodities', $commodity);
    }
    unset ($commodity);
  }
}

function dncaccounting_admin_report_type(array $form = [], array $form_state = []) {
  $max = dncstorage_get_max_id('accounts');
  if (empty($max)) {
    unset ($max);
    drupal_set_message(t('No account found yet'), 'error', FALSE);
    return $form;
  }
  
  $limit = 10;
  $current = pager_default_initialize($max, $limit);
  unset ($max);
  $current = $current * $limit;
  
  $accounts = dncstorage_get_data('accounts', NULL, [], FALSE, [$current, $limit]);
  $default = variable_get('dncaccounting_account_rtypes', []);
  
  $form['itable'] = [
    '#theme' => 'table',
    '#weight' => 10,
    '#rows' => [],
    '#header' => [
      'account' => ['data' => t('Account')],
      'normal' => [
        'data' => t('Normal'),
        'style' => 'text-align: center'
      ],
      'rtype' => [
        'data' => t('Report Type'),
        'style' => 'text-align: right'
      ]
    ]
  ];
  foreach ($accounts as $keys => $values) {
    $account = [
      '#id' => 'account-' . $keys . '-account',
      '#type' => 'item',
      '#markup' => '<strong>' . $values['title'] . '</strong>' . (empty($values['description']) ? NULL : '<br /><small>' . $values['description'] . '</small>')
    ];
    $normal = [
      '#id' => 'account-' . $keys . '-normal',
      '#type' => 'item',
      '#markup' => empty($values['normal']) ? t('Debit') : t('Credit')
    ];
    $rtype = [
      '#id' => 'account-' . $keys . '-type',
      '#type' => 'select',
      '#name' => 'typereport[' . $keys . ']',
      '#options' => [t('Balance Sheet'), ' -- ' . t('Profit and Lost')],
      '#value' => empty($default[$keys]) ? 0 : $default[$keys],
    ];
    $form['itable']['#rows'][$keys] = [
      'account' => ['data' => &$account],
      'normal' => [
        'data' => &$normal,
        'style' => 'text-align: ' . (empty($values['normal']) ? 'left' : 'right')
      ],
      'rtype' => [
        'data' => &$rtype,
        'style' => 'text-align: right'
      ]
    ];
    unset ($account, $normal, $rtype);
  }
  unset ($accounts, $default);
  
  $form['submit'] = [
    '#type' => 'submit',
    '#weight' => 100,
    '#value' => t('save'),
    '#attributes' => ['class' => ['btn', 'btn-success']],
    '#prefix' => '<div align="right">',
    '#suffix' => '</div>'
  ];
  
  $form['pager'] = [
    '#type' => 'item',
    '#weight' => $form['submit']['#weight'] + 1,
    '#markup' => theme('pager', ['quantity' => $limit])
  ];
  unset ($current, $limit);
  return $form;
}

function dncaccounting_admin_report_type_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
//  echo '<pre>'; print_r($form_state['input']); echo '</pre>'; exit;
  variable_set('dncaccounting_account_rtypes', $form_state['input']['typereport']);
  drupal_set_message(t('Configurations saved'));
}