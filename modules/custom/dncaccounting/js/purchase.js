var greferences = $.jStorage.get('accrefs', "default value");
//$.jStorage.deleteKey('accountingtrans');

var gjournals = {};

if (greferences.scenarios) {
  for (let i in greferences.scenarios) {
    id = parseInt(i);
    if (id < 4) {
      greferences.scenarios[i].detail.forEach(function(item) {
        gjournals[item.account] = {
          'account': item.account,
          'worth': 0
        };
      });
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
    scenario: "1",
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
        if (id < 4) {
          this.scenarios[i] = {
            'nid': i,
            'title': this.references.scenarios[i].title,
            'detail': []
          };
          this.references.scenarios[i].detail.forEach(function(item) {
            self.scenarios[i].detail.push({
              'account': item.account,
              'action': item.action,
            });
          });
        }
      }
      if (this.ttitle === '') {
        this.ttitle = this.scenarios[self.scenario].title;
      }
    }
    if (this.references.commodities) {
      var category = '';
      var commodity = '';
      this.commodities.amount = 0;
      this.commodities.data = {};
      for (let i in this.references.commodities) {
        this.commodities.amount++;
        this.references.commodities[i].nid = parseInt(i);
        category = this.references.commodities[i].category;
        if (!this.commodities.data[category])
          this.commodities.data[category] = {};
        this.commodities.data[category][i] = this.references.commodities[i];
      }
    }
  },
  computed: {
    jtotal: function() {
      var self = this;
      var total = {
        debit: 0,
        credit: 0
      };
      if (this.scenarios[this.scenario].detail) {
        this.scenarios[this.scenario].detail.forEach(function(item) {
          item.action = parseInt(item.action);
          if (item.action < 1) {
            total.debit += self.journals[item.account].worth;
          }
          else {
            total.credit += self.journals[item.account].worth;
          }
        });
      }
//      console.log(JSON.stringify([this.journals, total]));
      return total;
    },
    ballanced: function () {
      var ballanced = false;
      if (this.jtotal.debit > 0 && this.jtotal.credit > 0 && this.jtotal.debit === this.jtotal.credit) {
        ballanced = true;
      }
      return ballanced;
    },
    total: function() {
      var total = 0;
      
      for(let i in this.lineItems) {
        this.lineItems[i].price = Math.abs(this.lineItems[i].price);
        this.lineItems[i].numberOfItems = Math.abs(this.lineItems[i].numberOfItems);
        total += this.lineItems[i].price * this.lineItems[i].numberOfItems;
      }
      
      if (total > 0) {
        var self = this;
        var i = 0;
        this.scenarios[this.scenario].detail.forEach(function(item) {
          if (i < 2) {
            self.journals[item.account].worth = total;
          }
          i++;
        });
      }
      
      return total;
    },
    exactly: function () {
      var exact = false;
      if (this.total > 0 && this.jtotal.debit > 0 && this.total === this.jtotal.debit) {
        exact = true;
      }
      return exact;
    }
  },
  methods: {
    onItemClick: function(item) {
      var found = false;

      for (let i in this.lineItems) {
        if (this.lineItems[i].commodity === item.commodity) {
          this.lineItems[i].numberOfItems++;
          found = true;
          break;
        }
      }
      
      if (!found) {
        this.lineItems.push({ commodity: item, price: 0, numberOfItems: 1, editing: {price: false, numberOfItems: false } });
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
      for (let i in this.journals) {
        this.journals[i].worth = 0;
      }
      this.$forceUpdate();
    },
    collectAndSave: function() {
      var self = this;
      var tdump = $.jStorage.get('accountingtrans', []);
      this.transaction.time = this.ddatetime;
      this.transaction.title = this.ttitle;
      this.transaction.annotation = this.annotation;
      this.transaction.pic = this.pic;
      this.transaction.items = [];
      var o = 0;
      this.scenarios[this.scenario].detail.forEach(function(item) {
        var titems = {};
        titems.account = item.account;
        titems.action = item.action;
        titems.worth = self.journals[item.account].worth;
        self.transaction.items[o] = titems;
        o++;
      });
      if (this.lineItems) {
        this.transaction.stocks = [];
        for (var i = 0; i < this.lineItems.length; i++) {
          var tstocks = {};
          tstocks.commodity = this.lineItems[i].commodity.nid;
          tstocks.price = this.lineItems[i].price;
          tstocks.quantity = this.lineItems[i].numberOfItems;
          this.transaction.stocks[i] = tstocks;
        }
      }
      tdump.push(this.transaction);
      $.jStorage.set('accountingtrans', tdump);
      this.transaction = {};
      this.lineItems = [];
      this.ttitle = '';
      this.annotation = '';
      this.pic = '';
      this.ddatetime = '';
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
