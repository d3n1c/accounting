<?php

function dncaccounting_transaction_menu(&$items) {
  $items['dncaccounting/transaction'] = [
    'title' => 'Transaction',
    'description' => 'Recording transaction',
    'access arguments' => ['use dnc accounting'],
    'page callback' => 'dncaccounting_transaction_general',
    'file' => 'dncaccounting.transaction.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/transaction-post'] = [
    'title' => 'Transaction Post',
    'description' => 'Accounting action posting data transaction',
    'access arguments' => ['use dnc accounting'],
    'page callback' => 'dncaccounting_transaction_post',
    'file' => 'dncaccounting.transaction.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/purchase-transaction'] = [
    'title' => 'Purchase Transaction',
    'description' => 'Recording transaction of procurement',
    'access arguments' => ['use dnc accounting'],
    'page callback' => 'dncaccounting_purchase_transaction',
    'file' => 'dncaccounting.purchase.inc',
    'type' => MENU_CALLBACK,
  ];
  $items['dncaccounting/sale-transaction'] = [
    'title' => 'Sale Transaction',
    'description' => 'Recording transaction of sale',
    'access arguments' => ['use dnc accounting'],
    'page callback' => 'dncaccounting_sale_transaction',
    'file' => 'dncaccounting.sale.inc',
    'type' => MENU_CALLBACK,
  ];
}

function dncaccounting_transaction_general() {
  module_load_include('inc', 'dncaccounting', 'dncaccounting.reference');
  $references = dncaccounting_references_get_data(NULL, FALSE);
  
  //check references
  if (empty($references['accounts'])) {
    unset ($references);
    drupal_set_message(t('No references found'), 'error', FALSE);
    return '';
  }
  
  dncaccounting_prepare_transaction();
  
  drupal_add_js(drupal_get_path('module', 'dncaccounting') . '/js/app.js', array(
    'scope' => 'footer',
    'weight' => 135
  ));
  $script = '
    $.jStorage.set("accrefs", ' . json_encode($references) . ');
  ';
  drupal_add_js($script, array(
    'type' => 'inline',
    'scope' => 'footer',
    'weight' => 101
  ));
  unset ($script);
  
  $script = '
    $(document).ready(function(){
      $("#dncsearch").on("keyup", function() {
        var value = $(this).val().toLowerCase();
        $("#dnclists button").filter(function() {
          $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1)
        });
      });
    });
  ';
  drupal_add_js($script, array(
    'type' => 'inline',
    'scope' => 'footer',
    'weight' => 102
  ));
  unset ($script);

  $output = [];
  
  $output[] = '<div id="app">';
  $output[] = "\t" . '<div class="col-md-12" id="form-group" v-cloak>';
  $output[] = "\t\t" . '<div class="row">';
  $output[] = "\t\t\t" . '<div class="fade show date col-sm-6 col-md-2 col-lg-2" style="padding: 0 0 10px;">';
  $output[] = "\t\t\t\t" . '<input class="form-control dncdatepicker" v-model="ddatetime" type="text" placeholder="Choose Date">';
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '<div class="fade show date col-sm-6 col-md-6 col-lg-6" style="padding: 5px 10px;">';
  $output[] = "\t\t\t\t" . t('use format : year-month-day (YYYY-MM-DD)');
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '<div class="fade show col-12" style="padding: 0 0 10px;">';
  $output[] = "\t\t\t\t" . '<textarea class="form-control" cols="100%" rows="2" v-model="ttitle" placeholder="' . t('Title of transactions [required]') . '" required></textarea>';
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t" . '</div>';
  $output[] = "\t\t" . '<div class="row" v-if="ddatetime && ttitle">';
  $output[] = "\t\t\t" . '<div class="col-md-12 col-lg-12">';
  $output[] = "\t\t\t\t" . '<button type="button" class="btn btn-secondary" data-toggle="modal" data-target="#myModal">';
  $output[] = "\t\t\t\t\t" . t('Add Account');
  $output[] = "\t\t\t\t" . '</button>';
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '<div class="col col-md-12 col-lg-12">';
  $output[] = "\t\t\t\t" . '<table class="table table-striped table-hover">';
  $output[] = "\t\t\t\t" . '<thead v-if="lineItems.length">';
  $output[] = "\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t" . '<th>' . t('Name') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Debit') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Credit') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-center">' . t('Drop') . '</th>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t" . '</thead>';
  $output[] = "\t\t\t\t" . '<tbody v-if="lineItems.length">';
  $output[] = "\t\t\t\t\t" . '<tr v-for="item in lineItems">';
  $output[] = "\t\t\t\t\t\t" . '<td>{{ accounts[item.account].title }}</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span width="100%" style="cursor: pointer" v-if="!item.editdebit && item.debit < 1" v-on:click="toggleEdit(item, 0)">0</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span width="100%" style="cursor: pointer" v-if="!item.editdebit && item.debit > 0" v-on:click="toggleEdit(item, 0)">{{ item.debit | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<input v-if="item.editdebit" v-on:blur="toggleEdit(item, 0)" class="form-control" style="text-align: right;" type="number" v-model.number="item.debit">';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span width="100%" style="cursor: pointer" v-if="!item.editcredit && item.credit < 1" v-on:click="toggleEdit(item, 1)">0</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span width="100%" style="cursor: pointer" v-if="!item.editcredit && item.credit > 0" v-on:click="toggleEdit(item, 1)">{{ item.credit | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<input v-if="item.editcredit" v-on:blur="toggleEdit(item, 1)" class="form-control" style="text-align: right;" type="number" v-model.number="item.credit">';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-center"><i class="fa fa-times" style="cursor: pointer" v-on:click="removeItem(item.account)"></i></td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t" . '</table>';
  $output[] = "\t\t\t\t" . '<table class="table">';
  $output[] = "\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t" . '<tr v-if="ballanced"><td style="font-weight: bold">' . t('Total') . ':</td><td class="text-right" style="font-weight: bold; font-size: 24pt">{{ total.debit | decimal }}</td></tr>';
  $output[] = "\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t" . '</table>';
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '<div class="col-md-12" v-if="ballanced">';
  $output[] = "\t\t\t\t" . '<button type="button" class="btn btn-success" data-toggle="modal" data-target="#saveData">';
  $output[] = "\t\t\t\t\t" . t('Save');
  $output[] = "\t\t\t\t" . '</button> ';
  $output[] = "\t\t\t\t" . '<button type="button" class="btn btn-warning" v-on:click="resetElement()">';
  $output[] = "\t\t\t\t\t" . t('Reset');
  $output[] = "\t\t\t\t" . '</button> ';
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t" . '</div>';
  $output[] = "\t" . '</div>';

  // modal categories
  $output[] = "\t\t\t" . '<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">';
  $output[] = "\t\t\t\t" . '<div class="modal-dialog" role="document">';
  $output[] = "\t\t\t\t\t" . '<div class="modal-content">';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-header">';
  $output[] = "\t\t\t\t\t\t\t" . '<h4 class="modal-title">' . t('Accounts') . '</h4>';
  $output[] = "\t\t\t\t\t\t\t" . '<button type="button" class="close" data-dismiss="modal" aria-label="Close">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<span aria-hidden="true">&times;</span>';
  $output[] = "\t\t\t\t\t\t\t" . '</button>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-body">';
  
  if (count($references) > 10) {
    $output[] = "\t\t\t\t\t\t\t" . '<div class="col-12 search-wrapper form-group">';
    $output[] = "\t\t\t\t\t\t\t\t" . '<input class="input form-control" id="dncsearch" v-model="dncsearch" type="text" placeholder="Filter Search ...">';
    $output[] = "\t\t\t\t\t\t\t" . '</div>';
  }
  
  $output[] = "\t\t\t\t\t\t\t" . '<div class="list-group" id="dnclists">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<button class="list-group-item item text-left" v-for="(item, index) in accounts" v-on:click="onItemClick(index)" data-dismiss="modal">';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<strong>{{ item.title | sort }}</strong>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</button>';
  $output[] = "\t\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-footer">';
  $output[] = "\t\t\t\t\t\t\t" . '<button type="button" class="btn btn-secondary" data-dismiss="modal">' . t('Close') . '</button>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '</div>';

  // modal saveData
  $output[] = "\t\t\t" . '<div class="modal fade" id="saveData" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">';
  $output[] = "\t\t\t\t" . '<div class="modal-dialog" role="document">';
  $output[] = "\t\t\t\t\t" . '<div class="modal-content">';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-header">';
  $output[] = "\t\t\t\t\t\t\t" . '<h4 class="modal-title">' . t('Save Data') . '</h4>';
  $output[] = "\t\t\t\t\t\t\t" . '<button type="button" class="close" data-dismiss="modal" aria-label="Close">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<span aria-hidden="true">&times;</span>';
  $output[] = "\t\t\t\t\t\t\t" . '</button>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-body">';
  $output[] = "\t\t\t\t\t\t\t" . '<div class="list-group">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<h3>' . t('Confirmation') . ' *</h3>';
  $output[] = "\t\t\t\t\t\t\t\t" . t('Are you sure to posting this journal') . ' ?';
  $output[] = "\t\t\t\t\t\t\t\t" . '<h4>{{ ttitle }}</h4>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<small>' . t('Date') . ': {{ ddatetime }}</small>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<table class="table table-striped table-hover">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<thead v-if="lineItems.length">';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th>' . t('Name') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Debit') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Credit') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-center">' . t('Drop') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</thead>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<tbody v-if="lineItems.length">';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-for="item in lineItems">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td>{{ accounts[item.account].title }}</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span>{{ item.debit | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span>{{ item.credit | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-center"><i class="fa fa-times" v-on:click="removeItem(item.account)"></i></td>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</table>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<table class="table">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-if="ballanced"><td style="font-weight: bold">' . t('Total') . ':</td><td class="text-right" style="font-weight: bold; font-size: 24pt">{{ total.debit | decimal }}</td></tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</table>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<br /><br />';
  $output[] = "\t\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t\t" . '<div class="list-group">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<textarea class="form-control" cols="100%" rows="2" v-model="annotation"  placeholder="' . t('Annotation') . '"></textarea>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<input type="text" class="form-control" v-model="pic" placeholder="Person In Charge">';
  $output[] = "\t\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-footer">';
  $output[] = "\t\t\t\t\t\t\t" . '<button type="button" class="btn btn-success" data-dismiss="modal" v-on:click="collectAndSave()">' . t('Save') . '</button> ';
  $output[] = "\t\t\t\t\t\t\t" . '<button type="button" class="btn btn-warning" data-dismiss="modal">' . t('Cancel') . '</button>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '</div>';

  $output[] = '</div>';
  unset ($references);
  return implode("\n", $output);
}

function dncaccounting_transaction_save_transaction(array $sdata = []) {
  $result = [];
  if (empty($sdata['title']) || empty($sdata['items']) || empty($sdata['time'])) {
    unset ($sdata);
    $result['error'] = t('No title info or no items found');
    return $result;
  }
  $sdata['time'] = strtotime($sdata['time']);
  
  $is_sale = empty($sdata['sale']) ? FALSE : TRUE;

  // restructuring data
  module_load_include('inc', 'dncaccounting', 'dncaccounting.reference');
  $references = dncaccounting_references_get_data(NULL, FALSE);
  $itemdata = [];
  $sdata['total'] = 0;
  foreach ($sdata['items'] as $keys => $values) {
    if (empty($values['account']) || !isset($values['action']) || !isset($values['worth']) || empty($references['accounts'][$values['account']])) {
      continue;
    }
    $values['title'] = !empty($values['title']) ? $values['title'] : $references['accounts'][$values['account']]['title'];
    $values['time'] = $sdata['time'];
    $itemdata[$keys] = $values;
    if ($values['action'] < 1)
      $sdata['total'] += $values['worth'];
  }
  unset ($sdata['items']);
  
  if (!empty($sdata['stocks'])) {
    module_load_include('inc', 'dncaccounting', 'dncaccounting.stock');
    if (!empty($is_sale)) {
      $total = 0;
      $cgs = 0;
      $sdata['annotation'] = [
        'annotation' => $sdata['annotation'],
        'stocks' => []
      ];
      if (!empty($sdata['discount'])) {
        $sdata['annotation']['discount'] = $sdata['discount'];
      }
      if (!empty($sdata['addpay'])) {
        $sdata['annotation']['addpay'] = $sdata['addpay'];
      }
    }
    $datastocks = [];
    foreach ($sdata['stocks'] as $keys => $values) {
      if (empty($values['stocks'])) {
        if (empty($datastocks[$values['commodity']])) {
          $datastocks[$values['commodity']] = [
            'cost' => $values['price'],
            'quantity' => 0
          ];
        }
        $datastocks[$values['commodity']]['quantity'] += $values['quantity'];
      }
      else {
        $total += $values['price'] * $values['quantity'];
        $sdata['annotation']['stocks'][$keys] = [
          'commodity' => $values['commodity'],
          'price' => $values['price'],
          'quantity' => $values['quantity']
        ];
        $getstocks = dncaccounting_stock_get_data($values['commodity'], TRUE);
        if (!empty($getstocks)) {
          $datastocks[$values['commodity']] = $getstocks['stocks'];
          $res = 0;
          $count = 0;
          $sdump = [];
          $icount = 0;
          foreach ($getstocks['stocks'] as $key => $value) {
            if ($count < $values['quantity']) {
              $res = $values['quantity'] - $count;
              if ($value['quantity'] >= $res) {
                $count += $res;
                $datastocks[$values['commodity']][$key]['quantity'] -= $res;
                $cgs += $value['cost'] * $res;
              }
              else {
                $count += $value['quantity'];
                $datastocks[$values['commodity']][$key]['quantity'] -= $value['quantity'];
                $cgs += $value['cost'] * $value['quantity'];
              }
              $sdata['annotation']['stocks'][$keys]['use'][] = [
                'cost' => $value['cost'],
                'transaction' => $value['transaction'],
                'time' => $value['time']
              ];
            }
            else {
              break;
            }
            if (!empty($datastocks[$values['commodity']][$key]['quantity']))
              $sdump[] = $datastocks[$values['commodity']][$key];
          }
          $datastocks[$values['commodity']] = $sdump;
        }
        unset ($res, $count, $sdump, $getstocks);
      }
    }
    if (!empty($is_sale)) {
      $gconfigs = variable_get('dncaccounting_configs', []);
      $sdata['total'] = $total;
      $cash = $total;
      if (!empty($sdata['discount'])) {
        $cash -= ($total * $sdata['discount']) / 100;
      }
      if (!empty($gconfigs['show_addpay']) && !empty($sdata['addpay'])) {
        $cash += ($total * $sdata['addpay']) / 100;
      }
      foreach ($itemdata as $keys => $values) {
        if ($values['account'] == $references['accounts'][$gconfigs['accounts']['sale']]) {
          $itemdata[$keys]['worth'] = $total;
        }
        elseif ($values['account'] == $references['accounts'][$gconfigs['accounts']['cost_of_good_sold']]
            || $values['account'] == $references['accounts'][$gconfigs['accounts']['inventories']]
            ) {
          $itemdata[$keys]['worth'] = $cgs;
        }
        elseif ($values['account'] == $references['accounts'][$gconfigs['accounts']['cost']]) {
          if (!empty($sdata['discount'])) {
            $itemdata[$keys]['worth'] = ($total * $sdata['discount']) / 100;
          }
        }
        elseif ($values['account'] == $references['accounts'][$gconfigs['accounts']['debt']]) {
          if (!empty($gconfigs['show_addpay']) && !empty($sdata['addpay'])) {
            $itemdata[$keys]['worth'] = ($total * $sdata['addpay']) / 100;
          }
        }
        elseif ($values['account'] == $references['accounts'][$gconfigs['accounts']['cash']]) {
          $itemdata[$keys]['worth'] = $cash;
        }
      }
      unset ($gconfigs, $total, $cgs, $cash);
    }
  }
  
  $structure = dncaccounting_structures_of_data('transactions');
  foreach ($structure as $keys => $values) {
    if (!empty($values['required']) && empty($sdata[$keys])) {
      unset ($sdata, $structure, $itemdata, $itemnid);
      $result['error'] = t('@key element is required', array('@key' => $keys));
      return $result;
    }
    if (in_array($values['type'], array('integer', 'int', 'decimal')) && !empty($sdata[$keys])) {
      $numeric = preg_replace("/[^0-9\.]/", '', $sdata[$keys]);
      if (in_array($values['type'], ['integer', 'int'])) {
        settype($numeric, 'int');
      }
      else {
        settype($numeric, 'float');
        $divisor = empty($values['decimal_divisor']) ? [5, 2] : $values['decimal_divisor'];
        $comma = round($numeric, $divisor[1]);
        $numdiv = explode('.', $comma);
        $numeric = (strlen($numdiv[0]) + $divisor[1]) < $divisor[0] ? $comma : $numdiv[0];
        if ($numeric !== $comma) {
          settype($numeric, 'int');
        }
        unset ($numdiv, $comma, $divisor);
      }
      $sdata[$keys] = $numeric;
      unset ($numeric);
    }
    if ($values['type'] == 'reference' && !empty($sdata[$keys])) {
      $check = dncstorage_get_data($values['reference'], $sdata[$keys]);
      if (empty($check)) {
        $result['error'] = t('No reference by @val at @ref', array(
          '@val' => $sdata[$keys],
          '@ref' => $values['reference']
        ));
        unset ($check, $sdata, $structure, $itemdata, $itemnid);
        return $result;
      }
      unset ($check);
    }
  }
  unset ($structure);

  // adding point of sale
  unset ($sdata['stocks'], $sdata['discount'], $sdata['addpay']);
  $sdata = dncaccounting_set_data_type('transactions', $sdata);
  $sdata = dncaccounting_save_updated_data('transactions', $sdata);
  if (empty($sdata['nid'])) {
    unset ($sdata, $itemdata, $itemnid);
    $result['error'] = t('Saving transaction is failed');
    return $result;
  }

  /// updating item of sale's listing
  foreach ($itemdata as $keys => $values) {
    $values['transaction'] = $sdata['nid'];
    $values = dncaccounting_set_data_type('journals', $values);
    dncaccounting_save_updated_data('journals', $values);
  }
  
  if (!empty($datastocks)) {
    if (empty($is_sale)) {
      foreach ($datastocks as $keys => $values) {
        $datastocks[$keys]['transaction'] = $sdata['nid'];
        $datastocks[$keys]['time'] = $sdata['time'];
      }
      module_load_include('inc', 'dncaccounting', 'dncaccounting.stock');
      dncaccounting_stock_save_data($datastocks);
    }
    else {
//      watchdog('datastocks', '<pre>@print</pre>', ['@print' => print_r($datastocks, TRUE)]);
      foreach ($datastocks as $keys => $values) {
        $commodity = dncaccounting_get_data('commodities', $keys);
        $commodity['stocks'] = $values;
        $commodity = dncaccounting_set_data_type('commodities', $commodity);
        dncaccounting_save_updated_data('commodities', $commodity);
        unset ($commodity);
      }
    }
    unset ($datastocks);
  }
  
  unset ($sdata, $itemdata, $datastocks, $is_sale);

  return ['result' => TRUE];
}

function dncaccounting_transaction_post() {
  $result = [
    'result' => FALSE,
  ];
  if (!user_access('use dnc accounting') || empty($_POST['data'])) {
    $result['message'] = t('No data found or not authorized');
    drupal_json_output($result);
    drupal_exit();
  }

  $sdata = $_POST['data'];
  $updated = dncaccounting_transaction_save_transaction($sdata);
  unset ($sdata);
  if (!empty($updated['error'])) {
    $result['message'] = $updated['error'];
    drupal_json_output($result);
    drupal_exit();
  }
  unset ($updated);
  $result['result'] = TRUE;
  drupal_json_output($result);
}

