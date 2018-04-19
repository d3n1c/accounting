/* global Drupal */

var greferences = $.jStorage.get('accrefs', "default value");
//$.jStorage.deleteKey('accountingtrans');

var gjournals = {};

if (greferences.scenarios) {
  for (let i in greferences.scenarios) {
    id = parseInt(i);
    if (greferences.scenarios[i].detail && id > 3 && id < 7) {
      for (let u in greferences.scenarios[i].detail) {
        gjournals[greferences.scenarios[i].detail[u].account] = {
          'account': greferences.scenarios[i].detail[u].account,
          'worth': 0
        };
      }
    }
  }
}

Vue.use('money', {precision: 4});
Vue.directive('money', VMoney.Money.directives.money);
var app =  new Vue({
  el: '#app',
  data: {
    references: greferences,
    commodities: {},
    journals: gjournals,
    scenarios: {},
    scenario: "4",
    lineItems: [],
    ttitle: '',
    transaction: {},
    message: '',
    errormsg: '',
    dncsearch: '',
    annotation: '',
    pic: '',
    ddatetime: '',
    loading: '',
    discount: {
      title: '',
      percentage: 0,
      editing: false
    },
    addpay: {
      title: '',
      percentage: 0,
      editing: false
    },
    money: {
      decimal: ',',
      thousands: '.',
      prefix: '',
      suffix: '',
      precision: 0,
      masked: false
    }
  },
  components: {'money' : VMoney.Money},
  created: function() {
    var self = this;
    if (this.references.scenarios) {
      var id = 0;
      for (let i in this.references.scenarios) {
        id = parseInt(i);
        if (this.references.scenarios[i].detail && id > 3 && id < 7) {
          this.scenarios[i] = {
            'nid': i,
            'title': this.references.scenarios[i].title,
            'detail': []
          };
          for (let u in self.references.scenarios[i].detail) {
            if (self.references.scenarios[i].detail[u]) {
              self.scenarios[i].detail[u] = {
                'account': self.references.scenarios[i].detail[u].account,
                'action': self.references.scenarios[i].detail[u].action,
              };
            }
          }
        }
      }
      if (this.ttitle === '') {
        this.ttitle = this.scenarios[this.scenario].title;
      }
    }
    if (this.references.commodities) {
      this.comrebuild(this.references.commodities);
    }
  },
  computed: {
    total: function() {
      var total = 0;
      var item = {};
      for(let i in this.lineItems) {
        item = this.lineItems[i];
        item.price = Math.abs(item.price);
        item.numberOfItems = Math.abs(item.numberOfItems);
        total += item.price * item.numberOfItems;
      }
      this.updatejournal(Drupal.settings.accounts.sale, total);
      return total;
    },
    grandtotal: function () {
      var gt = this.total;
      var vdiscount = 0;
      var vaddpay = 0;
      if (this.discount.percentage > 0) {
        vdiscount = (this.total * this.discount.percentage) / 100;
      }
      if (this.addpay.percentage > 0) {
        vaddpay = (this.total * this.addpay.percentage) / 100;
      }
      gt -= vdiscount;
      gt += vaddpay;
      
      if (gt > 0) {
        if (this.scenarios[this.scenario].detail) {
          var item = {};
          var litem = {};
          var cgs = 0;
          var countcgs = 0;
          var res = 0;
          var acntnames = {
            cash: Drupal.settings.accounts.cash.toString(),
            inventories: Drupal.settings.accounts.inventories.toString(),
            cost_of_good_sold: Drupal.settings.accounts.cost_of_good_sold.toString(),
          };
          for (let i in this.scenarios[this.scenario].detail) {
            item = this.scenarios[this.scenario].detail[i];
            if (item.account === acntnames.cash) {
              this.updatejournal(item.account, gt);
            }
            else if (item.account === acntnames.inventories || item.account === acntnames.cost_of_good_sold) {
              cgs = 0;
              for(let u in this.lineItems) {
                litem = this.lineItems[u];
                countcgs = 0;
                for (var e = 0; e < litem.commodity.stocks.length; e++) {
                  if (countcgs < litem.numberOfItems) {
                    res = litem.numberOfItems - countcgs;
                    if (litem.commodity.stocks[e].quantity >= res) {
                      countcgs += res;
                      cgs += res * litem.commodity.stocks[e].cost;
                    }
                    else {
                      countcgs += litem.commodity.stocks[e].quantity;
                      cgs += litem.commodity.stocks[e].quantity * litem.commodity.stocks[e].cost;
                    }
                  }
                  else {
                    break;
                  }
                }
              }
              this.updatejournal(item.account, cgs);
            }
          }
        }
      }
      
      return gt;
    },
    jtotal: function() {
      var self = this;
      var jtotal = {
        debit: 0,
        credit: 0
      };
      if (this.scenarios[this.scenario].detail) {
        var item = {};
        for (let i in this.scenarios[this.scenario].detail) {
          item = this.scenarios[this.scenario].detail[i];
          item.action = parseInt(item.action);
          if (item.action < 1) {
            jtotal.debit += self.journals[item.account].worth;
          }
          else {
            jtotal.credit += self.journals[item.account].worth;
          }
        }
        
        if (this.discount.percentage > 0) {
          jtotal.debit += (this.total * this.discount.percentage) / 100;
        }
        if (Drupal.settings.show_addpay && this.addpay.percentage > 0) {
          jtotal.credit += (this.total * this.addpay.percentage) / 100;
        }
      };
      return jtotal;
    },
    ballanced: function () {
      var ballanced = false;
      if (this.jtotal.debit > 0 && this.jtotal.credit > 0 && this.jtotal.debit === this.jtotal.credit) {
        ballanced = true;
      }
      return ballanced;
    },
    isreadonly: function() {
      var jr = {};
      var dc = {
        debit: 0,
        credit: 0
      };
      if (this.scenarios[this.scenario].detail) {
        var item = {};
        for (let i in this.scenarios[this.scenario].detail) {
          item = this.scenarios[this.scenario].detail[i];
          if (!jr[item.account]) {
            jr[item.account] = false;
          }
          if (item.account === Drupal.settings.accounts.cost_of_good_sold.toString() || item.account === Drupal.settings.accounts.inventories.toString()) {
            jr[item.account] = true;
          }
          item.action = parseInt(item.action);
          if (item.action < 1) {
            dc.debit++;
          }
          else {
            dc.credit++;
          }
        }
        if (dc.debit === dc.credit) {
          for (let i in jr) {
            jr[i] = true;
          }
        }
      }
      return jr;
    }
  },
  methods: {
    comrebuild: function (commreferences) {
      var category = '';
      var commodity = '';
      this.commodities.amount = 0;
      this.commodities.data = {};
      for (let i in commreferences) {
        if (commreferences[i].price && commreferences[i].price > 0) {
          this.commodities.amount++;
          commreferences[i].nid = parseInt(i);
          category = commreferences[i].category;
          if (!this.commodities.data[category])
            this.commodities.data[category] = {};
          this.commodities.data[category][i] = commreferences[i];
        }
      }
    },
    updatejournal: function(account, worth) {
      account = account.toString();
      if (!this.journals[account]) {
        this.journals[account] = {
          account: account,
          worth: 0
        };
      }
      this.journals[account].worth = worth;
    },
    onItemClick: function(item) {
      var found = false;

      for (let i in this.lineItems) {
        if (this.lineItems[i].commodity === item) {
          this.lineItems[i].numberOfItems++;
          found = true;
          break;
        }
      }
      
      if (!found) {
        item.price = parseInt(item.price);
        this.lineItems.push({ commodity: item, price: item.price, numberOfItems: 1, editing: {price: false, numberOfItems: false } });
      }
    },
    toggleEdit: function(lineItem, point) {
      lineItem.editing[point] = !lineItem.editing[point];
      if (point === 'price') {
        lineItem.editing.numberOfItems = false;
      }
      else {
        lineItem.editing.price = false;
      }
    },
    discountEdit: function() {
      this.discount.editing = !this.discount.editing;
    },
    addpayEdit: function() {
      this.addpay.editing = !this.addpay.editing;
    },
    removeItem: function(lineItem) {
      for (var i = 0; i < this.lineItems.length; i++) {
        if (this.lineItems[i].commodity === lineItem) {
          this.lineItems.splice(i, 1);
          break;
        }
      }
    },
    resetElement: function() {
      this.transaction = {};
      this.lineItems = [];
      this.ttitle = '';
      this.annotation = '';
      this.pic = '';
      this.ddatetime = '';
      this.discount = {
        title: '',
        percentage: 0,
        editing: false
      };
      this.addpay = {
        title: '',
        percentage: 0,
        editing: false
      };
      for (let i in this.journals) {
        this.journals[i].worth = 0;
      }
      this.$forceUpdate();
    },
    collectAndSave: function() {
      var self = this;
      var tdump = $.jStorage.get('accountingtrans', []);
      this.transaction.sale = true;
      this.transaction.time = this.ddatetime;
      this.transaction.title = this.ttitle;
      this.transaction.pic = this.pic;
      this.transaction.items = [];
      this.transaction.annotation = this.annotation;
      var o = 0;
      var titems = {};
      var item = {};
      var rcommodities = this.references.commodities;
      for (let i in this.scenarios[this.scenario].detail) {
        item = this.scenarios[this.scenario].detail[i];
        titems.account = item.account;
        titems.action = item.action;
        titems.worth = self.journals[item.account].worth;
        self.transaction.items[o] = titems;
        titems = {};
        o++;
      }
      if (this.lineItems) {
        var gettotal = 0;
        var sales = [];
        var tstocks = {
          commodity: 0,
          quantity: 0,
          price: 0,
          stocks: []
        };
        var countcgs = 0;
        var res = 0;
        var stockuse = {};
        for (var i = 0; i < this.lineItems.length; i++) {
          tstocks.commodity = this.lineItems[i].commodity.nid;
          tstocks.quantity = this.lineItems[i].numberOfItems;
          tstocks.price = this.lineItems[i].price;
          gettotal += this.lineItems[i].price * this.lineItems[i].numberOfItems;
          countcgs = 0;
          res = 0;
          stockuse = {
            stock: {},
            quantity: 0
          };
          for (let e in this.lineItems[i].commodity.stocks) {
            if (countcgs < this.lineItems[i].numberOfItems) {
              res = this.lineItems[i].numberOfItems - countcgs;
              stockuse.stock = this.lineItems[i].commodity.stocks[e];
              if (this.lineItems[i].commodity.stocks[e].quantity >= res) {
                countcgs += res;
                stockuse.quantity = res;
                this.lineItems[i].commodity.stocks[e].quantity -= res;
                rcommodities[this.lineItems[i].commodity.nid].quantity -= res;
              }
              else {
                countcgs += litem.commodity.stocks[e].quantity;
                stockuse.quantity = this.lineItems[i].commodity.stocks[e].quantity;
                this.lineItems[i].commodity.stocks[e].quantity = 0;
                rcommodities[this.lineItems[i].commodity.nid].quantity -= litem.commodity.stocks[e].quantity;
              }
              tstocks.stocks.push(stockuse);
              rcommodities[this.lineItems[i].commodity.nid].stocks[e] = this.lineItems[i].commodity.stocks[e];
            }
            else {
              break;
            }
          }
          sales.push(tstocks);
          tstocks = {
            commodity: 0,
            quantity: 0,
            price: 0,
            stocks: []
          };
        }
        this.transaction.stocks = sales;
        
        if (this.discount.percentage > 0) {
          this.transaction.discount = this.discount.percentage;
          titems.account = Drupal.settings.accounts.cost;
          titems.title = this.references.accounts[titems.account].title;
          titems.title += ' discount';
          titems.title += this.discount.title != '' ? ' ' + this.discount.title : '';
          titems.action = 0;
          titems.worth = (gettotal * this.discount.percentage) / 100;
          self.transaction.items[o] = titems;
          titems = {};
          o++;
        }
        if (Drupal.settings.show_addpay && this.addpay.percentage > 0) {
          this.transaction.addpay = this.addpay.percentage;
          titems.account = Drupal.settings.accounts.debt;
          titems.title = this.references.accounts[titems.account].title;
          titems.title += this.addpay.title != '' ? ' ' + this.addpay.title : '';
          titems.action = 0;
          titems.worth = (gettotal * this.addpay.percentage) / 100;
          self.transaction.items[o] = titems;
          titems = {};
          o++
        }
      }
      tdump.push(this.transaction);
//      console.log(tdump);
      $.jStorage.set('accountingtrans', tdump);
      this.references.commodities = rcommodities;
      $.jStorage.set("accrefs", this.references);
      this.comrebuild(rcommodities);
      this.transaction = {};
      this.lineItems = [];
      this.ttitle = '';
      this.annotation = '';
      this.pic = '';
      this.ddatetime = '';
      this.discount = {
        title: '',
        percentage: 0,
        editing: false
      };
      this.addpay = {
        title: '',
        percentage: 0,
        editing: false
      };
      for (let i in this.journals) {
        this.journals[i].worth = 0;
      }
      $.notify(
        'Transaction is saved',
        {
          className:"success",
          globalPosition:"top center"
        }
      );
      this.$forceUpdate();
    }
  },
  mounted: function() {
    $(".dncdatepicker").datepicker({
      autoclose: true,
      format: 'yyyy-mm-dd',
      orientation: "bottom"
    }).on('changeDate', () => {
      this.ddatetime = $(".dncdatepicker").val();
    });
  },
  filters: {
    currency: function (value) {
      if (!value) return '';
      if (typeof value !== 'number') return value;
      var formatter = new Intl.NumberFormat('id-ID', {
        style: 'currency',
        currency: 'IDR',
        minimumFractionDigits: 0
      });
      return formatter.format(value);
    },
    decimal: function (value) {
      if (!value || typeof value !== 'number') return '';
      var formatter = new Intl.NumberFormat('id-ID', {
        style: 'decimal',
        minimumFractionDigits: 0
      });
      return formatter.format(value);
    }
  }
});
