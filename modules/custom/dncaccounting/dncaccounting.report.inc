<?php

function dncaccounting_report_menu(&$items) {
  $items['dncaccounting/report/journals'] = [
    'title' => 'Journals',
    'description' => 'Journals of transactions',
    'access arguments' => ['view dnc accounting'],
    'page callback' => 'dncaccounting_report_transactions',
    'file' => 'dncaccounting.report.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/report/balances'] = [
    'title' => 'Balances',
    'description' => 'Balances of accounts',
    'access arguments' => ['view dnc accounting'],
    'page callback' => 'dncaccounting_report_balances',
    'file' => 'dncaccounting.report.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/report/balances/%dncaccounting'] = [
    'title' => 'Balances Detail',
    'description' => 'Detail of account\'s balances',
    'access arguments' => ['view dnc accounting'],
    'page callback' => 'dncaccounting_report_balances_detail',
    'page arguments' => [3],
    'file' => 'dncaccounting.report.inc',
    'type' => MENU_VISIBLE_IN_BREADCRUMB,
  ];
  $items['dncaccounting/report/balance-sheet'] = [
    'title' => 'Balance Sheet',
    'description' => 'Balance sheet of accounts',
    'access arguments' => ['view dnc accounting'],
    'page callback' => 'dncaccounting_report_balance_sheet',
    'file' => 'dncaccounting.report.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/report/profit-lost'] = [
    'title' => 'Profit or Lost',
    'description' => 'Report of profit or lost',
    'access arguments' => ['view dnc accounting'],
    'page callback' => 'dncaccounting_report_profit_lost',
    'file' => 'dncaccounting.report.inc',
    'type' => MENU_CALLBACK,
  ];
}

function dncaccounting_report_transactions() {
  $output = [];
  
  $total = dncstorage_get_listing('transactions', [], [], TRUE, TRUE);
  if (empty($total)) {
    unset ($total);
    drupal_set_message(t('No transaction found yet'), 'error', FALSE);
    return implode("\n", $output);
  }
  
  $limit = 10;
  $current = pager_default_initialize($total, $limit);
  $current = $current * $limit;
  unset ($total);
  
  $transactions = dncstorage_get_listing('transactions', [], [$current, $limit], FALSE, TRUE);
  unset ($current);
//  dncaccounting_prepare_datatables_js();
  $table = [
    'rows' => [],
    'header' => [
      'date' => [
        'data' => t('Date'),
        'style' => 'text-align: right'
      ],
      'transaction' => ['data' => t('Transaction')],
      'debit' => [
        'data' => t('Debit'),
        'style' => 'text-align: right'
      ],
      'credit' => [
        'data' => t('Credit'),
        'style' => 'text-align: right'
      ]
    ],
    'attributes' => ['id' => 'dncaccounting-table']
  ];
  $i = 0;
  foreach ($transactions as $keys => $values) {
    $journals = dncaccounting_get_data('journals', NULL, TRUE, [
      'type' => 'field',
      'fields' => ['transaction' => $values['nid']]
    ], FALSE);
    if (empty($journals)) {
      unset ($journals);
      continue;
    }
    $table['rows'][$i] = [
      'date' => [
        'data' => dncsystem_date('j F Y', $values['time']),
        'style' => 'text-align: right',
      ],
      'transaction' => ['data' => $values['title'] . (empty($values['annotation']) ? NULL : '<br /><small>' . $values['annotation'] . '</small>')],
      'debit' => ['data' => NULL],
      'credit' => ['data' => NULL]
    ];
    
    $debit = [];
    $credit = [];
    
    foreach ($journals as $key => $value) {
      $debit[] = empty($value['action']) ? $value['title'] . ': <strong>' . number_format($value['worth'], 0, ',', '.') . '</strong>' : '&nbsp;';
      $credit[] = !empty($value['action']) ? $value['title'] . ': <strong>' . number_format($value['worth'], 0, ',', '.') . '</strong>' : '&nbsp;';
    }
    unset ($journals);
    $table['rows'][$i]['debit'] = [
      'data' => implode('<br />', $debit),
      'style' => 'text-align: right'
    ];
    $table['rows'][$i]['credit'] = [
      'data' => implode('<br />', $credit),
      'style' => 'text-align: right'
    ];
    $i++;
  }
  unset ($transactions);
//  echo '<pre>'; print_r($table); echo '</pre>'; exit;
  $output[] = theme('table', $table);
  unset ($table);
  $output[] = theme('pager', ['quantity' => $limit]);
  unset ($limit);
  return implode("\n", $output);
}

function dncaccounting_report_recount_balance(array $form = [], array $form_state = []) {
  $script = '
    $(document).ready(function() {
      $("#report-recounting").click();
    });
  ';
  drupal_add_js($script, [
    'type' => 'inline',
    'scope' => 'footer'
  ]);
  $form['info'] = [
    '#type' => 'item',
    '#weight' => 10,
    '#markup' => t('Recounting acount\'s balances')
  ];
  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('execute'),
    '#weight' => 20,
    '#attributes' => [
      'id' => 'report-recounting',
      'class' => ['btn', 'btn-success']
    ]
  ];
  return $form;
}

function dncaccounting_report_recount_balance_submit() {
  $tlast = variable_get('dncaccounting_lastT_balance', 0);
  $max = dncstorage_get_max_id('transactions');
  if ($tlast >= $max) {
    unset ($tlast, $max);
    return;
  }
  
  $operations = [];
  for ($i = $tlast; $i < $max; $i++) {
    $operations[] = ['dncaccounting_recount_balance_exec', [$i + 1]];
  }
  
  $batch_definition = [
    'operations' => $operations,
    'title' => t('Dumping Data Sources'),
    'init_message' => t('Progress start...'),
    'progress_message' => t('Precessed @current out of @total'),
    'error_message' => t('Process has encountered an error'),
    'finished' => 'dncaccounting_recount_balance_finished'
  ];
  unset ($operations);
  batch_set($batch_definition);
}

function dncaccounting_report_prepare_balance() {
  //check report file
  $file = 'public://dncaccounting_balances.json';
  $file = drupal_realpath($file);
  clearstatcache();
  $tlast = variable_get('dncaccounting_lastT_balance', 0);
  $max = dncstorage_get_max_id('transactions');
  if ($tlast > $max || !is_file($file)) {
    $tlast = 0;
    dncaccounting_account_balances(NULL, TRUE);
  }
  
  if ($tlast < $max) {
    if ($tlast < 1) {
      if (is_file($file)) {
        unlink($file);
      }
    }
    unset($tlast, $max);
    $batchform = drupal_get_form('dncaccounting_report_recount_balance');
    $output[] = drupal_render($batchform);
    unset ($batchform);
    return implode("\n", $output);
  }
  unset ($tlast, $max, $file);
  return;
}

function dncaccounting_report_balances() {
  $output = dncaccounting_report_prepare_balance();
  if (!empty($output)) {
    return $output;
  }
  $output = [];
  $data = dncaccounting_account_balances();
  if (empty($data)) {
    unset ($data);
    $output[] = t('No single transaction has occurred');
    return implode("\n", $output);
  }
  dncaccounting_prepare_datatables_js();
  $table = [
    'rows' => [],
    'header' => [
      'account' => ['data' => t('Account')],
      'balance' => [
        'data' => t('Balance'),
        'style' => 'text-align: right'
      ]
    ],
    'attributes' => ['id' => 'dncaccounting-table']
  ];
  foreach ($data as $keys => $values) {
    $account = dncaccounting_get_data('accounts', $keys);
    if (empty($account['title'])) {
      unset ($account);
      continue;
    }
    $table['rows'][] = [
      'account' => ['data' => l($account['title'], 'dncaccounting/report/balances/' . $keys)],
      'balance' => [
        'data' => number_format($values, 0, ',', '.'),
        'style' => 'text-align: right',
        'data-order' => $values
      ]
    ];
    unset ($account);
  }
  unset ($data);
  $output[] = theme('table', $table);
  unset ($table);
  return implode("\n", $output);
}

function dncaccounting_report_balances_detail($account) {
  $output = [];
  $account = dncaccounting_get_data('accounts', $account);
  if (empty($account['title'])) {
    return implode("\n", $output);
  }
  $output[] = '<h3><strong>' . $account['title'] . '</strong></h3>';
  $jmax = dncstorage_get_max_id('journals');
  if (empty($jmax)) {
    return implode("\n", $output);
  }
  
  $breadcrumb = array();
  $breadcrumb[] = l(t('home'), '<front>');
  $pmenu = menu_link_get_preferred('dncaccounting/report/balances');
  $breadcrumb[] = l(t($pmenu['link_title']), $pmenu['link_path']);
  unset ($pmenu);
  drupal_set_breadcrumb($breadcrumb);
  unset ($breadcrumb);
  
  dncaccounting_prepare_datatables_js();
  $table = [
    'rows' => [],
    'header' => [
      'date' => [
        'data' => t('Date'),
        'style' => 'text-align: right'
      ],
      'transaction' => ['data' => t('Transaction')],
      'debit' => [
        'data' => t('Debit'),
        'style' => 'text-align: right'
      ],
      'credit' => [
        'data' => t('Credit'),
        'style' => 'text-align: right'
      ],
      'saldo' => [
        'data' => t('Balance'),
        'style' => 'text-align: right'
      ]
    ],
    'attributes' => ['id' => 'dncaccounting-table']
  ]; 
  $saldo = 0;
  for ($i = 1; $i <= $jmax; $i++) {
    $journal = dncaccounting_get_data('journals', $i);
    if (!empty($journal['account']) && $journal['account'] == $account['nid']) {
      $transaction = dncaccounting_get_data('transactions', $journal['transaction']);
      if (empty($transaction['title'])) {
        unset($transaction, $journal);
        continue;
      }
      if ($journal['action'] == $account['normal']) {
        $saldo += $journal['worth'];
      }
      else {
        $saldo -= $journal['worth'];
      }
      $transaction['annotation'] = is_array($transaction['annotation']) ? (empty($transaction['annotation']['annotation']) ? NULL : $transaction['annotation']['annotation']) : $transaction['annotation'];
      $table['rows'][] = [
        'date' => [
          'data' => dncsystem_date('j F Y', $journal['time']),
          'style' => 'text-align: right',
          'data-order' => $journal['time']
        ],
        'transaction' => ['data' => $transaction['title'] . (empty($transaction['annotation']) ? NULL : '<br /><small>' . $transaction['annotation'] . '</small>')],
        'debit' => [
          'data' => empty($journal['action']) ? number_format($journal['worth'], 0, ',', '.') : NULL,
          'style' => 'text-align: right',
          'data-order' => empty($journal['action']) ? $journal['worth'] : 0
        ],
        'credit' => [
          'data' => !empty($journal['action']) ? number_format($journal['worth'], 0, ',', '.') : NULL,
          'style' => 'text-align: right',
          'data-order' => !empty($journal['action']) ? $journal['worth'] : 0
        ],
        'saldo' => [
          'data' => number_format($saldo, 0, ',', '.'),
          'style' => 'text-align: right',
          'data-order' => $saldo
        ]
      ];
      unset ($transaction);
    }
    unset ($journal);
  }
  $output[] = '<h4>' . t('Ending Balance') . ': <strong>' . number_format($saldo, 0, ',', '.') . '</strong></h4>';
  unset ($saldo);
  $output[] = theme('table', $table);
  unset ($table);
  return implode("\n", $output);
}

function dncaccounting_report_balance_sheet() {
  $output = dncaccounting_report_prepare_balance();
  if (!empty($output)) {
    return $output;
  }
  $output = [];
  $rtypes = variable_get('dncaccounting_account_rtypes', []);
  if (empty($rtypes)) {
    unset ($rtypes);
    drupal_set_message(t('Not configured yet, contact administrator please ...'), 'warning', FALSE);
    return implode("\n", $output);
  }
  
//  dncaccounting_prepare_datatables_js();
  $balances = dncaccounting_account_balances();
  
  $table = [
    'rows' => [],
    'attributes' => ['id' => 'dncaccounting-table'],
    'header' => [
      'account' => ['data' => t('Account')],
      'debit' => [
        'data' => t('Debit'),
        'style' => 'text-align: right'
      ],
      'credit' => [
        'data' => t('Credit'),
        'style' => 'text-align: right'
      ]
    ]
  ];
  $total = [
    'debit' => 0,
    'credit' => 0
  ];
  foreach ($rtypes as $keys => $values) {
    $account = dncaccounting_get_data('accounts', $keys);
    if (!empty($values) || empty($account['title'])) {
      continue;
    }
    settype($keys, 'int');
    $table['rows'][$keys]['account']['data'] = $account['title'];
    $table['rows'][$keys]['debit']['data'] = !empty($account['normal']) || empty($balances[$keys]) ? '-' : number_format($balances[$keys], 0, ',', '.');
    $table['rows'][$keys]['credit']['data'] = empty($account['normal']) || empty($balances[$keys]) ? '-' : number_format($balances[$keys], 0, ',', '.');
    $table['rows'][$keys]['debit']['style'] = 'text-align: right';
    $table['rows'][$keys]['credit']['style'] = 'text-align: right';
    if (empty($account['normal'])) {
      $total['debit'] += empty($balances[$keys]) ? 0 : $balances[$keys];
    }
    else {
      $total['credit'] += empty($balances[$keys]) ? 0 : $balances[$keys];
    }
    unset ($account);
  }
  unset ($rtypes);
  
  if ($total['debit'] != $total['credit']) {
    $table['rows'][888888]['account']['data'] = '<span' . ($total['debit'] < $total['credit'] ? ' style="color: red">' . t('Lost') : '>' . t('Profit')) . '</span>';
    if ($total['debit'] < $total['credit']) {
      $table['rows'][888888]['debit']['data'] = '<span style="color: red">' . number_format(($total['credit'] - $total['debit']), 0, ',', '.') . '</span>';
      $table['rows'][888888]['credit']['data'] = '-';
      $total['debit'] += $total['credit'] - $total['debit'];
    }
    else {
      $table['rows'][888888]['debit']['data'] = '-';
      $table['rows'][888888]['credit']['data'] = '<span>' . number_format(($total['debit'] - $total['credit']), 0, ',', '.') . '</span>';
      $total['credit'] += $total['debit'] - $total['credit'];
    }
    foreach ($table['header'] as $keys => $values) {
      if ($keys != 'account') {
        $table['rows'][888888][$keys]['style'] = 'text-align: right';
      }
    }
  }
  
  $table['rows'][999999]['account']['data'] = '<strong>' . t('Total') . '</strong>';
  $table['rows'][999999]['debit']['data'] = '<strong>' . number_format($total['debit'], 0, ',', '.') . '</strong>';
  $table['rows'][999999]['credit']['data'] = '<strong>' . number_format($total['credit'], 0, ',', '.') . '</strong>';
  foreach ($table['header'] as $keys => $values) {
    $table['rows'][999999][$keys]['style'] = 'text-align: right';
  }
  unset ($total);
  
  $output[] = theme('table', $table);
  unset ($table);
  
  return implode("\n", $output);
}

function dncaccounting_report_profit_lost() {
  $output = [];
  
  $rtypes = variable_get('dncaccounting_account_rtypes', []);
  if (empty($rtypes)) {
    unset ($rtypes);
    drupal_set_message(t('Not configured yet, contact administrator please ...'), 'warning', FALSE);
    return implode("\n", $output);
  }
  
  $balances = dncaccounting_account_balances();
  $dump = [];
  foreach ($rtypes as $keys => $values) {
    $account = dncaccounting_get_data('accounts', $keys);
    if (empty($values) || empty($account['title']) || empty($balances[$keys])) {
      continue;
    }
    $dump[$account['normal'] . '-' . $account['title']] = [
      'account' => $account,
      'worth' => $balances[$keys]
    ];
    unset ($account);
  }
  krsort($dump);
  $data = [];
  foreach ($dump as $values) {
    $data[] = $values;
  }
  unset ($dump);
  
  $table = [
    'rows' => [],
    'attributes' => ['id' => 'dncaccounting-table'],
    'header' => [
      'account' => ['data' => ''],
      'debit' => [
        'data' => '',
        'style' => 'text-align: right'
      ],
      'credit' => [
        'data' => '',
        'style' => 'text-align: right'
      ]
    ]
  ];
  $total = [
    'debit' => 0,
    'credit' => 0
  ];
  foreach ($data as $keys => $values) {
    $table['rows'][$keys]['account']['data'] = $values['account']['title'];
    $table['rows'][$keys]['debit']['data'] = !empty($values['account']['normal']) ? '' : number_format($values['worth'], 0, ',', '.');
    $table['rows'][$keys]['credit']['data'] = empty($values['account']['normal']) ? '' : number_format($values['worth'], 0, ',', '.');
    
    $table['rows'][$keys]['debit']['style'] = 'text-align: right';
    $table['rows'][$keys]['credit']['style'] = 'text-align: right';
    if (empty($values['account']['normal'])) {
      $total['debit'] += $values['worth'];
    }
    else {
      $total['credit'] += $values['worth'];
    }
  }
  unset ($data);
  
  $table['rows'][777777]['account']['data'] = t('Total Cost of Good Sold');
  $table['rows'][777777]['debit']['data'] = '';
  $table['rows'][777777]['credit']['data'] = number_format($total['debit'], 0, ',', '.');
  $table['rows'][777777]['debit']['style'] = 'text-align: right';
  $table['rows'][777777]['credit']['style'] = 'text-align: right';
  
  $lop = $total['credit'] - $total['debit'];
  $table['rows'][888888]['account']['data'] = '<strong' . ($lop < 0 ? ' style="color: red">' . t('Lost') : '>' . t('Profit')) . '</strong>';
  $table['rows'][888888]['debit']['data'] = '';
  $table['rows'][888888]['credit']['data'] = '<strong' . ($lop < 0 ? ' style="color: red">' : '>') . number_format($lop, 0, ',', '.') . '</strong>';
  $table['rows'][888888]['debit']['style'] = 'text-align: right';
  $table['rows'][888888]['credit']['style'] = 'text-align: right';
  
  unset ($lop);
  unset ($total);
  
  $output[] = theme('table', $table);
  unset ($table);
  
  return implode("\n", $output);
}
