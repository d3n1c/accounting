var references = $.jStorage.get('accrefs', "default value")
//$.jStorage.deleteKey('accountingtrans')
var app =  new Vue({
  el: '#app',
  data: {
    accounts: references.accounts,
    lineItems: [],
    ttitle: '',
    transaction: {},
    message: '',
    errormsg: '',
    dncsearch: '',
    annotation: '',
    pic: '',
    ddatetime: ''
  },
//  created: function() {
//    var d = new Date();
//    var dyear = d.getFullYear();
//    var dmonth = (d.getMonth() + 1);
//    var dday = d.getDate();
//    this.datetime = dyear + '-' + (dmonth.length < 2 ? '0' : '') + dmonth + '-' + (dday.length < 2 ? '0' : '') + dday;
//  },
  computed: {
    total: function() {
      var total = {
        debit: 0,
        credit: 0
      };
      this.lineItems.forEach(function(item) {
        item.debit = parseInt(item.debit);
        item.credit = parseInt(item.credit);
        total.debit += item.debit;
        total.credit += item.credit;
      });
      return total;
    },
    ballanced: function () {
      var ballanced = false;
      if (this.total.debit > 0 && this.total.credit > 0 && this.total.debit === this.total.credit) {
        ballanced = true;
      }
      return ballanced;
    }
  },
  mounted: function() {
    $(".dncdatepicker").datepicker({
      autoclose: true,
      format: 'yyyy-mm-dd',
      zIndexOffset: 99999,
      orientation: "bottom"
    }).on('changeDate', () => {
      this.ddatetime = $(".dncdatepicker").val();
    });
  },
  methods: {
    onItemClick: function(item) {
      var found = false;

      for (let i in this.lineItems) {
        if (this.lineItems[i].account === item) {
          found = true;
          break;
        }
      }
      
      if (!found) {
        this.lineItems.push({ account: item, debit: 0, credit: 0, editdebit: false, editcredit: false });
      }
    },
    toggleEdit: function(lineItem, point) {
      if (point > 0) {
        lineItem.editcredit = !lineItem.editcredit;
        lineItem.editdebit = false;
        lineItem.debit = 0;
      }
      else {
        lineItem.editdebit = !lineItem.editdebit;
        lineItem.editcredit = false;
        lineItem.credit = 0;
      }
    },
    removeItem: function(lineItem) {
      for (var i = 0; i < this.lineItems.length; i++) {
        if (this.lineItems[i].account === lineItem) {
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
    },
    collectAndSave: function() {
      var tdump = $.jStorage.get('accountingtrans', []);
      this.transaction.time = this.ddatetime;
      this.transaction.title = this.ttitle;
      this.transaction.annotation = this.annotation;
      this.transaction.pic = this.pic;
      if (this.lineItems) {
        this.transaction.items = [];
        for (var i = 0; i < this.lineItems.length; i++) {
          var titems = {};
          titems.account = this.lineItems[i].account;
          titems.action = this.lineItems[i].debit ? 0 : 1;
          titems.worth = this.lineItems[i].debit ? this.lineItems[i].debit : this.lineItems[i].credit;
          this.transaction.items[i] = titems;
        }
      }
      tdump.push(this.transaction);
//      console.log(JSON.stringify([tdump]));
      $.jStorage.set('accountingtrans', tdump);
      this.transaction = {};
      this.lineItems = [];
      this.ttitle = '';
      this.annotation = '';
      this.pic = '';
      this.ddatetime = '';
      $.notify(
        'Transaction is saved',
        {
          className:"success",
          globalPosition:"top center"
        }
      );
    }
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
})
