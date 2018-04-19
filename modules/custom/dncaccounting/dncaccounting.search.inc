<?php

function dncaccounting_search_for_autocomplete($type, $string) {
  $matches = [];
  $result = dncstorage_get_data($type, NULL, [
    'type' => 'string',
    'string' => $string
  ], FALSE, [0, 10]);
//  watchdog('search', '<pre>@print</pre>', array('@print' => print_r($result, TRUE)));
  
  if (!empty($result)) {
    foreach ($result as $keys => $values) {
      $title = $values['title'] . (empty($values['description']) ? NULL : '; ' . $values['description']);
      $matches[$title . ' | ' . $keys] = $title;
      unset ($title);
    }
  }
  drupal_json_output($matches);
}

