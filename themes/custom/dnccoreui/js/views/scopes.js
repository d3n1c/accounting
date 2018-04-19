$(function(){
  'use strict';

  function dncGetRemote(t) {
    var response = null;
    $.get('http://localhost:8081/neomorowali/service-api/reqdata?type=' + t, function(data, status){
      if (status == 200) {
        response = data;
      }
    });
    return response;
  }

  
});
