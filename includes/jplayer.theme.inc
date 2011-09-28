<?php

/**
 * Theme and preprocess functions for the jPlayer module.
 */

/**
 * Themes single file.
 */
function theme_jplayer_formatter_single($element) {
  if ($element['#item']['fid']) {
    jplayer_add();
    return theme('jplayer_single', $element);
  }
  return '';
}

/**
 * Preprocess function for jplayer.tpl.php when doing a single item.
 */
function template_preprocess_jplayer_single(&$vars) {
  $vars['mode'] = 'single';
  $vars['item'] = array(
    'url' => file_create_url($vars['element']['#item']['filepath']),
    'label' => !empty($vars['element']['#item']['data']['description']) ? $vars['element']['#item']['data']['description'] : $vars['element']['#item']['filename'],
  );
  $vars['player_id'] = _jplayer_check_id('jplayer-' . $vars['element']['#node']->nid . '-' . str_replace('_', '-', $vars['element']['#field_name']) . '-' . $vars['element']['#item']['#delta']);

}

/**
 * Themes multiple items as playlist.
 */
function theme_jplayer_formatter_playlist($element) {
  if ($element[0]['#item']['fid']) {
    jplayer_add();
    // If there's more than one file, use the playlist player. Otherwise, fall
    // back to the regular player.
    if (isset($element[1])) {
      return theme('jplayer_playlist', $element);
    }
    $element += $element[0];
    return theme('jplayer_single', $element);
  }
  return '';
}

/**
 * Preprocess function for jplayer.tpl.php when using a playlist.
 */
function template_preprocess_jplayer_playlist(&$vars) {
  $vars['mode'] = 'playlist';
  $vars['items'] = array();
  $children = element_children($vars['element']);
  $count = count($children);
  $number = 0;
  foreach ($children as $key) {
    $classes = array();
    if ($number == 0) {
      $classes[] = 'first';
      $classes[] = 'jplayer_playlist_item_first';
    }
    if ($number == $count - 1) {
      $classes[] = 'last';
      $classes[] = 'jplayer_playlist_item_last';
    }
    $vars['items'][] = array(
      'class' => implode(' ', $classes),
      'url' => file_create_url($vars['element'][$key]['#item']['filepath']),
      'label' => !empty($vars['element'][$key]['#item']['data']['description']) ? $vars['element'][$key]['#item']['data']['description'] : $vars['element'][$key]['#item']['filename'],
    );
    $number++;
  }

  $vars['player_id'] = _jplayer_check_id('jplayer-' . $vars['element']['#node']->nid . '-' . str_replace('_', '-', $vars['element']['#field_name']));
}

/**
 * Preprocess function for jplayer.tpl.php when displaying a view as a playlist.
 */
function template_preprocess_jplayer_view_playlist(&$vars) {
  if (empty($vars['items'])) {
    return;
  }

  jplayer_add();

  $count = count($vars['items']);
  foreach ($vars['items'] as $number => $item) {
    $classes = array();
    if ($number == 0) {
      $classes[] = 'first';
      $classes[] = 'jplayer_playlist_item_first';
    }
    if ($number == $count - 1) {
      $classes[] = 'last';
      $classes[] = 'jplayer_playlist_item_last';
    }
    $vars['items'][$number]['class'] = implode(' ', $classes);
  }

  $vars['mode'] = 'playlist';
  $vars['player_id'] = _jplayer_check_id('jplayer-view-' . str_replace('_', '-', $vars['view']->name));
}

/**
 * Return a unique ID for a jPlayer. This allows multiple embedded jPlayers to
 * point to an identical file on the same page.
 *
 * @param $id
 *   The ID to check for uniqueness.
 *
 * @return
 *   A modified ID if the ID was not unique, or the same ID passed in if it was
 *   unique.
 */
function _jplayer_check_id($id) {
  // We keep track of player IDs generated per request. This ensures that if a
  // player pointing to the same field is shown multiple times on a page, that
  // each player gets a unique ID. This is especially a problem when jPlayers
  // are embedded in hidden content such as View rendered with Quicktabs.
  static $player_ids = array();

  // Add the request time, so if the same player is inserted multiple times
  // AJAX all players are functional.
  $id = $id . '-' . $_SERVER['REQUEST_TIME'];

  // Store a count of the number of times a unique ID is used, and make it
  // unique if needed.
  if (isset($player_ids[$id])) {
    $id = $id . '-' . $player_ids[$id]++;
  }
  else {
    $player_ids[$id] = 0;
  }

  return $id;
}

