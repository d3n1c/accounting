<?php

function dncaccounting_sale_transaction() {
  module_load_include('inc', 'dncaccounting', 'dncaccounting.reference');
  $references = dncaccounting_references_get_data(NULL, FALSE);
  
  //check references
  if (empty($references['accounts']) || empty($references['categories']) || empty($references['commodities'])) {
    unset ($references);
    drupal_set_message(t('No references found'), 'error', FALSE);
    return '';
  }
  
  $dncaccounting = variable_get('dncaccounting_configs');
  module_load_include('inc', 'dncaccounting', 'dncaccounting.stock');
  $commodities = [];
  foreach ($references['commodities'] as $keys => $values) {
    $stocks = dncaccounting_stock_get_data($keys, TRUE);
    if (!empty($stocks['quantity']) && !empty($stocks['price']) && !empty($stocks['stocks'])) {
      if (!empty($dncaccounting['stock_sale_flow'])) {
        $dump = [];
        foreach ($stocks['stocks'] as $key => $value) {
          $dump[] = $value;
        }
        $stocks['stocks'] = $dump;
        unset ($dump);
      }
      $values['stocks'] = $stocks['stocks'];
      $values['quantity'] = $stocks['quantity'];
      $values['nid'] = $keys;
      $commodities[$keys] = $values;
    }
    unset ($stocks);
  }
  $references['commodities'] = $commodities;
  unset ($commodities);
//  echo '<pre>'; print_r($references['commodities']); echo '</pre>'; exit;
  
  if (empty($references['commodities'])) {
    unset ($references, $dncaccounting);
    drupal_set_message(t('No any commodities found'), 'error', FALSE);
    return '';
  }
  
  dncaccounting_prepare_transaction();
  drupal_add_js($dncaccounting, 'setting');
  
  $script = '
    $.jStorage.set("accrefs", ' . json_encode($references) . ');
  ';
  drupal_add_js($script, [
    'type' => 'inline',
    'scope' => 'footer',
    'weight' => 100
  ]);
  unset ($script);
  
  drupal_add_js(drupal_get_path('module', 'dncaccounting') . '/js/sale.js', array(
    'scope' => 'footer',
    'weight' => 127
  ));
  
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
  
  $script = '
    [v-cloak] {
      display: none;
    }
    #overlay {
        position: fixed;
        width: 100%;
        height: 100%;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: rgba(0,0,0,0.5);
        z-index: 20;
        cursor: pointer;
    }
    .overlay-body {
        position: absolute;
        top: 50%;
        left: 50%;
        font-size: 50px;
        color: white;
        transform: translate(-50%,-50%);
        -ms-transform: translate(-50%,-50%);
        text-align: center;
    }
  ';
  drupal_add_css($script, array(
    'type' => 'inline',
  ));
  unset ($script);

  $output = [];
  
  $output[] = '<div id="app">';
  
  // required elements
  $output[] = "\t" . '<div class="col-md-12" id="form-group" v-cloak>';
  $output[] = "\t\t" . '<div class="row">';
  $output[] = "\t\t\t" . '<div class="fade show col-12" style="padding: 0 0 10px;">';
  $output[] = "\t\t\t\t" . '<label>' . t('Sale Type') . '</label>';
  $output[] = "\t\t\t\t" . '<select class="mb-3" v-model="scenario" required>';
  $output[] = "\t\t\t\t" . '<option v-for="item in scenarios" :value="item.nid">{{ item.title }}</option>';
  $output[] = "\t\t\t\t" . '</select>';
  $output[] = "\t\t\t" . '</div>';
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
  
  // commodities
  $output[] = "\t\t" . '<div class="row" v-if="ddatetime && ttitle && commodities">';
  $output[] = "\t\t\t" . '<div class="col-md-12 col-lg-12">';
  $output[] = "\t\t\t\t" . '<button type="button" class="btn btn-secondary" data-toggle="modal" data-target="#myModal">';
  $output[] = "\t\t\t\t\t" . t('Add Commodity');
  $output[] = "\t\t\t\t" . '</button>';
//  $output[] = '
//    <div>
//      <pre>{{ Drupal.settings }}</pre>
//    </div>
//  ';
  $output[] = "\t\t\t" . '</div>';
  $output[] = "\t\t\t" . '<div class="col-md-12 col-lg-12" v-if="ddatetime && ttitle && lineItems.length">';
  $output[] = "\t\t\t\t" . '<table class="table table-striped table-hover">';
  $output[] = "\t\t\t\t" . '<thead>';
  $output[] = "\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t" . '<th>' . t('Name') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Price') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Quantity') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Subtotal') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-center">' . t('Remove') . '</th>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t" . '</thead>';
  $output[] = "\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t" . '<tr v-for="item in lineItems">';
  $output[] = "\t\t\t\t\t\t" . '<td>{{ commodities.data[item.commodity.category][item.commodity.nid].title }}</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!item.editing.price && item.price < 1" v-on:click="toggleEdit(item, \'price\')">0</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!item.editing.price && item.price > 0" v-on:click="toggleEdit(item, \'price\')">{{ item.price | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<input v-if="item.editing.price" v-on:blur="toggleEdit(item, \'price\')" type="number" v-model.number="item.price" style="text-align: right;">';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!item.editing.numberOfItems && item.numberOfItems < 1" v-on:click="toggleEdit(item, \'numberOfItems\')">0</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!item.editing.numberOfItems && item.numberOfItems > 0" v-on:click="toggleEdit(item, \'numberOfItems\')">{{ item.numberOfItems | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<input v-if="item.editing.numberOfItems" v-on:blur="toggleEdit(item, \'numberOfItems\')" type="number" v-model.number="item.numberOfItems" style="text-align: right;">';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">{{ item.numberOfItems * item.price | decimal }}</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-center"><i class="fa fa-times" style="cursor: pointer" v-on:click="removeItem(item.commodity)"></i></td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t" . '</table>';
  $output[] = "\t\t\t\t" . '<table class="table" v-if="total > 0">';
  $output[] = "\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t" . '<td><strong>' . t('Total') . ':</strong></td>';
  $output[] = "\t\t\t\t\t\t" . '<td></td>';
  $output[] = "\t\t\t\t\t\t" . '<td></td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right"><strong>{{ total | currency }}</strong></td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t" . '<td><strong>' . t('Discount') . '</strong></td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<input type="text" v-model="discount.title" class="form-control" placeholder="' . t('annotation') . '">';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!discount.editing && discount.percentage < 1" v-on:click="discountEdit()">0</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!discount.editing && discount.percentage > 0" v-on:click="discountEdit()">{{ discount.percentage | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<input v-if="discount.editing" v-on:blur="discountEdit()" type="number" v-model.number="discount.percentage" style="text-align: right;" class="form-control">%';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '{{ (total * discount.percentage) / 100 | decimal }}';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t" . '<tr v-if="Drupal.settings.show_addpay">';
  $output[] = "\t\t\t\t\t\t" . '<td><strong>' . t('Addpay') . '</strong></td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<input type="text" v-model="addpay.title" class="form-control" placeholder="' . t('annotation') . '">';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!addpay.editing && addpay.percentage < 1" v-on:click="addpayEdit()">0</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span style="cursor: pointer;" v-if="!addpay.editing && addpay.percentage > 0" v-on:click="addpayEdit()">{{ addpay.percentage | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<input v-if="addpay.editing" v-on:blur="addpayEdit()" type="number" v-model.number="addpay.percentage" style="text-align: right;" class="form-control">%';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '{{ (total * addpay.percentage) / 100 | decimal }}';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t" . '<td><strong>' . t('Grand Total') . ':</strong></td>';
  $output[] = "\t\t\t\t\t\t" . '<td></td>';
  $output[] = "\t\t\t\t\t\t" . '<td></td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right"><strong>{{ grandtotal | currency }}</strong></td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t" . '</table>';
  $output[] = "\t\t\t" . '</div>';
//  unset ($references); return implode("\n", $output);
  
  // journals
  $output[] = "\t\t\t" . '<div class="col col-md-12 col-lg-12" v-if="total > 0 && scenarios[scenario].detail">';
  $output[] = "\t\t\t\t" . '<hr />';
  $output[] = "\t\t\t\t" . '<label><h4><strong>' . t('Journal') . '</strong></h4></label>';
  $output[] = "\t\t\t\t" . '<table class="table table-striped table-hover">';
  $output[] = "\t\t\t\t" . '<thead>';
  $output[] = "\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t" . '<th>' . t('Account') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Debit') . '</th>';
  $output[] = "\t\t\t\t\t\t" . '<th class="text-right">' . t('Credit') . '</th>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t" . '</thead>';
  $output[] = "\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t" . '<tr v-for="item in scenarios[scenario].detail">';
  $output[] = "\t\t\t\t\t\t" . '<td>{{ references.accounts[item.account].title }}</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span v-if="item.action > 0"> </span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span v-if="item.action < 1 && isreadonly[item.account]">{{ journals[item.account].worth | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t" . '<money v-if="!isreadonly[item.account] && item.action < 1" class="form-control" style="text-align: right;" v-model="journals[item.account].worth" v-bind="money" :value="journals[item.account].worth"></money>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span v-if="item.action < 1"> </span>';
  $output[] = "\t\t\t\t\t\t\t" . '<span v-if="item.action > 0">{{ journals[item.account].worth | decimal }}</span>';
//  $output[] = "\t\t\t\t\t\t\t" . '<money v-if="item.action > 0" class="form-control" style="text-align: right;" v-model="journals[item.account].worth" v-bind="money" :value="journals[item.account].worth" readonly="true"></money>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t" . '<tr v-if="discount.percentage > 0">';
  $output[] = "\t\t\t\t\t\t" . '<td>';
  $output[] = "\t\t\t\t\t\t" . '{{ references.accounts[' . $dncaccounting['accounts']['cost'] . '].title }} ' . t('discount');
  $output[] = "\t\t\t\t\t\t" . '<span v-if="discount.title">{{ discount.title }}</span>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span v-if="discount.percentage">{{ (total * discount.percentage) / 100 | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span> </span>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t" . '<tr v-if="addpay.percentage > 0">';
  $output[] = "\t\t\t\t\t\t" . '<td>';
  $output[] = "\t\t\t\t\t\t" . '{{ references.accounts[' . $dncaccounting['accounts']['debt'] . '].title }}';
  $output[] = "\t\t\t\t\t\t" . '<span v-if="addpay.title">{{ addpay.title }}</span>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span> </span>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t" . '<span v-if="addpay.percentage">{{ (total * addpay.percentage) / 100 | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t" . '<tr v-if="!ballanced"><td style="color: red;">' . t('Unballanced debit and credit') . ':</strong></td><td></td><td class="text-right" style="color: red;">{{ jtotal.debit - jtotal.credit | currency }}</td></tr>';
  $output[] = "\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t" . '</table>';
  $output[] = "\t\t\t" . '</div>';
  
  // confirm and saving
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
  $output[] = "\t\t\t\t\t\t\t" . '<h4 class="modal-title">' . t('Commodities') . '</h4>';
  $output[] = "\t\t\t\t\t\t\t" . '<button type="button" class="close" data-dismiss="modal" aria-label="Close">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<span aria-hidden="true">&times;</span>';
  $output[] = "\t\t\t\t\t\t\t" . '</button>';
  $output[] = "\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-body pre-scrollable">';
  
//  $output[] = "\t\t\t\t\t\t\t" . '<div class="col-12 search-wrapper form-group" v-if="commodities.amount> 5">';
//  $output[] = "\t\t\t\t\t\t\t\t" . '<input class="input form-control" id="dncsearch" v-model="dncsearch" type="text" placeholder="Filter Search ...">';
//  $output[] = "\t\t\t\t\t\t\t" . '</div>';
  
  $output[] = "\t\t\t\t\t\t\t" . '<div class="list-group col-12" id="dnclists" v-if="commodities.data">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<div class="list-item" v-for="(values, keys) in commodities.data">';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<h4>{{ references.categories[keys].title }}</h4>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<div class="list-group" v-if="values">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<button class="list-group-item item text-left" v-for="item in values" v-on:click="onItemClick(item)" data-dismiss="modal">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<strong>{{ item.title }}</strong>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</button>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</div>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</div>';
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
  $output[] = "\t\t\t\t\t\t" . '<div class="modal-body pre-scrollable">';
  $output[] = "\t\t\t\t\t\t\t" . '<div class="list-group">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<h3>' . t('Confirmation') . ' *</h3>';
  $output[] = "\t\t\t\t\t\t\t\t" . t('Are you sure to posting this journal') . ' ?';
  $output[] = "\t\t\t\t\t\t\t\t" . '<h4>{{ ttitle }}</h4>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<small>' . t('Date') . ': {{ ddatetime }}</small>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<table class="table table-striped table-hover">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<thead>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th>' . t('Name') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Debit') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Credit') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</thead>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-for="item in scenarios[scenario].detail">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td>{{ references.accounts[item.account].title }}</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span v-if="item.action < 1">{{ journals[item.account].worth | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span v-if="item.action > 0">{{ journals[item.account].worth | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-if="discount.percentage > 0">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td>{{ references.accounts[Drupal.settings.accounts.cost.toString()].title }}';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . ' ' . t('discount') . ' <span v-if="discount.title">{{ discount.title }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span>{{ (total * discount.percentage) / 100 | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span> </span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-if="addpay.percentage > 0">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td>{{ references.accounts[Drupal.settings.accounts.debt.toString()].title }}';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . ' <span v-if="addpay.title">{{ addpay.title }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span> </span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span>{{ (total * addpay.percentage) / 100 | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</table>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<hr /><label>' . t('Commodities') . '</label>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<table class="table table-striped table-hover">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<thead>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th>' . t('Name') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Price') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Quantity') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<th class="text-right">' . t('Subtotal') . '</th>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</thead>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-for="item in lineItems">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td>{{ commodities.data[item.commodity.category][item.commodity.nid].title }}</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span>{{ item.price | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">';
  $output[] = "\t\t\t\t\t\t\t\t\t\t\t" . '<span>{{ item.numberOfItems | decimal }}</span>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t\t" . '<td class="text-right">{{ item.numberOfItems * item.price | decimal }}</td>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '</tr>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</tbody>';
  $output[] = "\t\t\t\t\t\t\t\t" . '</table>';
  $output[] = "\t\t\t\t\t\t\t\t" . '<table class="table">';
  $output[] = "\t\t\t\t\t\t\t\t" . '<tbody>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr><td><strong>' . t('Total') . ':</strong></td><td class="text-right"><strong>{{ total | currency }}</strong></td></tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-if="discount.percentage > 0"><td><strong>' . t('Discount') . ':</strong> <span v-if="discount.title">{{ discount.title }} ({{ discount.percentage }}%)</span></td><td class="text-right"><strong>{{ (total * discount.percentage) / 100 | currency }}</strong></td></tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr v-if="addpay.percentage > 0"><td><strong>' . t('Addpay') . ':</strong> <span v-if="addpay.title">{{ addpay.title }}</span></td><td class="text-right"><strong>{{ (total * addpay.percentage) / 100 | currency }}</strong></td></tr>';
  $output[] = "\t\t\t\t\t\t\t\t\t" . '<tr><td><strong>' . t('Grant Total') . ':</strong></td><td class="text-right"><strong>{{ grandtotal | currency }}</strong></td></tr>';
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