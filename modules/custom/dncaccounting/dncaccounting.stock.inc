<?php

function dncaccounting_stock_menu(&$items) {
  $items['dncaccounting/report/stocks'] = [
    'title' => 'Stocks',
    'description' => 'Listing of stocks',
    'access arguments' => ['use dnc accounting'],
    'page callback' => 'dncaccounting_stock_report',
    'file' => 'dncaccounting.stock.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/report/stocks/%dncaccounting'] = [
    'title' => 'Stock Detail',
    'description' => 'Detail of stock',
    'access arguments' => ['use dnc accounting'],
    'page callback' => 'dncaccounting_stock_report_detail',
    'page arguments' => [3],
    'file' => 'dncaccounting.stock.inc',
    'type' => MENU_VISIBLE_IN_BREADCRUMB,
  ];
}

function dncaccounting_stock_get_data($commodity, $detail = FALSE) {
  $data = dncaccounting_get_data('commodities', $commodity);
  if (empty($data)) {
    unset ($data);
    return;
  }
  
  $data['quantity'] = 0;
  if (!empty($data['stocks'])) {
    foreach ($data['stocks'] as $keys => $values) {
      $data['quantity'] += $values['quantity'];
    }
  }
  
  if (empty($detail)) {
    unset ($data['stocks']);
  }
  
  return $data;
}

function dncaccounting_stock_save_data(array $data = []) {
  if (empty($data)) {
    return;
  }
  
  foreach ($data as $keys => $values) {
    $commodity = dncaccounting_get_data('commodities', $keys);
    if (empty($commodity['nid'])
        || !isset($values['transaction'])
        || !isset($values['time'])
        || !isset($values['cost'])
        || !isset($values['quantity'])
        ) {
      unset ($commodity);
      continue;
    }
    foreach ($values as $key => $value) {
      if (!in_array($key, ['transaction', 'time', 'cost', 'quantity'])) {
        unset ($values[$key]);
      }
    }
    if (empty($commodity['stocks'])) {
      $commodity['stocks'] = [];
    }
    else {
      $dump = [];
      foreach($commodity['stocks'] as $key => $value) {
        $dump[] = $value;
      }
      $commodity['stocks'] = $dump;
      unset ($dump);
    }
    $commodity['stocks'][] = $values;
    $commodity = dncaccounting_set_data_type('commodities', $commodity);
    dncaccounting_save_updated_data('commodities', $commodity);
    unset ($commodity);
  }
  
  return TRUE;
}

function dncaccounting_stock_report() {
  $output = [];
  $data = dncaccounting_get_data('commodities');
  if (empty($data)) {
    unset ($data);
    return implode("\n", $output);
  }
  dncaccounting_prepare_datatables_js();
  $table = [
    'rows' => [],
    'header' => [
      'category' => ['data' => t('Category')],
      'title' => ['data' => t('Title')],
      'price' => [
        'data' => t('Price'),
        'style' => 'text-align: right'
      ],
      'quantity' => [
        'data' => t('Quantity'),
        'style' => 'text-align: right'
      ]
    ],
    'attributes' => ['id' => 'dncaccounting-table']
  ];
  foreach ($data as $keys => $values) {
    $stock = dncaccounting_stock_get_data($keys);
    if (empty($stock['quantity'])) {
      unset ($stock);
      continue;
    }
    $values['price'] = empty($values['price']) ? 0 : $values['price'];
    $table['rows'][] = [
      'category' => ['data' => '<strong>' . $values['category']['title'] . '</strong>'],
      'commodity' => ['data' => l($values['title'], 'dncaccounting/report/stocks/' . $keys) . (empty($values['description']) ? NULL : '<br /><small>' . $values['description'] . '</small>')],
      'price' => [
        'data' => 'Rp. ' . number_format($values['price'], 0, ',', '.'),
        'style' => 'text-align: right',
        'data-order' => $values['price']
      ],
      'quantity' => [
        'data' => number_format($stock['quantity'], 0, ',', '.') . ' ' . $values['unit'],
        'style' => 'text-align: right',
        'data-order' => $stock['quantity']
      ]
    ];
    unset ($stock);
  }
  unset ($data);
  $output[] = theme('table', $table);
  unset ($table);
  return implode("\n", $output);
}

function dncaccounting_stock_report_detail($commodity) {
  $output = [];
  $data = dncaccounting_get_data('commodities', $commodity);
  $stocks = dncaccounting_stock_get_data($commodity, TRUE);
  if (empty($data) || empty($stocks['quantity'])) {
    unset ($data, $stocks);
    drupal_set_message(t('No data found by the ID or no stock available'), 'error', FALSE);
    return implode("\n", $output);
  }
  
  $breadcrumb = array();
  $breadcrumb[] = l(t('home'), '<front>');
  $pmenu = menu_link_get_preferred('dncaccounting/report/stocks');
  $breadcrumb[] = l(t($pmenu['link_title']), $pmenu['link_path']);
  unset ($pmenu);
  drupal_set_breadcrumb($breadcrumb);
  unset ($breadcrumb);
  
  $data['price'] = empty($data['price']) ? 0 : $data['price'];
  $category = dncaccounting_get_data('categories', $data['category']);
  $output[] = '<h3><strong>' . $data['title'] . '</strong></h3>';
  $output[] = '<br /><strong>' . t('Category') . ':</strong> ' . $category['title'];
  $output[] = '<br /><strong>' . t('Price') . ':</strong> Rp. ' . number_format($data['price'], 0, ',', '.');
  $output[] = '<br /><strong>' . t('Available Quantity') . ':</strong> ' . number_format($stocks['quantity'], 0, ',', '.') . ' ' . $data['unit'];
  
  dncaccounting_prepare_datatables_js();
  $table = [
    'rows' => [],
    'header' => [
      'time' => [
        'data' => t('Date'),
        'style' => 'text-align: right'
      ],
      'transaction' => ['data' => t('Transaction')],
      'cost' => [
        'data' => t('Purchase Cost'),
        'style' => 'text-align: right'
      ],
      'stock' => [
        'data' => t('Stock'),
        'style' => 'text-align: right'
      ]
    ],
    'attributes' => ['id' => 'dncaccounting-table']
  ];
  foreach ($stocks['stocks'] as $keys => $values) {
    $transaction = dncaccounting_get_data('transactions', $values['transaction']);
    $table['rows'][$keys] = [
      'time' => [
        'data' => dncsystem_date('j F Y', $values['time']),
        'style' => 'text-align: right'
      ],
      'transaction' => ['data' => $transaction['title']],
      'cost' => [
        'data' => 'Rp. ' . number_format($values['cost'], 0, ',', '.'),
        'style' => 'text-align: right',
      ],
      'stock' => [
        'data' => number_format($values['quantity'], 0, ',', '.') . ' ' . $data['unit'],
        'style' => 'text-align: right'
      ]
    ];
    unset ($transaction);
  }
  $output[] = '<br /><br /><strong>' . t('Purchase Transactions') . ':</strong><br />';
  $output[] = theme('table', $table);
  unset ($table);
  
  unset ($data, $category);
  return implode("\n", $output);
}