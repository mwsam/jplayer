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
  $player = jplayer_sort_files(array($vars['item']), $vars['player_id'], $vars['mode']);
  $vars['playlist'] = theme('jplayer_item_list', $player['playlist']);
  $vars['type'] = $player['type'];
  $player = array(
    'jplayerInstances' => array(
      $vars['player_id'] => array(
        'files' => $player['files'],
        'autoplay' => (int) variable_get('jplayer_autoplay', ''),
        'supplied' => $player['extensions'],
      ),
    ),
  );
  drupal_add_js($player, 'setting');
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
  $player = jplayer_sort_files($vars['items'], $vars['player_id'], $vars['mode']);
  $vars['playlist'] = theme('jplayer_item_list', $player['playlist']);
  $vars['type'] = $player['type'];
  $player = array(
    'jplayerInstances' => array(
      $vars['player_id'] => array(
        'files' => $player['files'],
        'autoplay' => (int) variable_get('jplayer_autoplay', ''),
        'supplied' => $player['extensions'],
      ),
    ),
  );
  drupal_add_js($player, 'setting');
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
  $player = jplayer_sort_files($vars['items'], $vars['player_id'], $vars['mode']);
  $vars['playlist'] = theme('jplayer_item_list', $player['playlist']);
  $vars['type'] = $player['type'];
  $player = array(
    'jplayerInstances' => array(
      $vars['player_id'] => array(
        'files' => $player['files'],
        'autoplay' => (int) variable_get('jplayer_autoplay', ''),
        'supplied' => $player['extensions'],
      ),
    ),
  );
  drupal_add_js($player, 'setting');
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

/**
 * Modified version of theme_item_list().
 */
function theme_jplayer_item_list($items, $title = NULL, $type = 'ul', $attributes = array()) {
  $output = '';
  if (isset($title)) {
    $output .= '<h3>' . $title . '</h3>';
  }

  if (!empty($items)) {
    $output .= "<$type" . drupal_attributes($attributes) . '>';
    $num_items = count($items);
    foreach ($items as $i => $item) {
      $attributes = array();
      $children = array();
      if (is_array($item)) {
        foreach ($item as $key => $value) {
          if ($key == 'data') {
            $data = $value;
          }
          elseif ($key == 'children') {
            $children = $value;
          }
          else {
            $attributes[$key] = $value;
          }
        }
      }
      else {
        $data = $item;
      }
      if (count($children) > 0) {
        $data .= theme_item_list($children, NULL, $type, $attributes); // Render nested list
      }
      if ($i == 0) {
        $attributes['class'] = empty($attributes['class']) ? 'first' : ($attributes['class'] . ' first');
        $attributes['class'] .= ' jp-playlist-first';
      }
      if ($i == $num_items - 1) {
        $attributes['class'] = empty($attributes['class']) ? 'last' : ($attributes['class'] . ' last');
        $attributes['class'] .= ' jp-playlist-last';
      }
      $output .= '<li' . drupal_attributes($attributes) . '>' . $data . "</li>\n";
    }
    $output .= "</$type>";
  }
  $output .= '</div>';
  return $output;
}

/**
 * Filters an array of files into groups ready for jPlayer.
 */
function jplayer_sort_files($raw_files = array(), $player_id = '', $type = 'single') {
  $video_extensions = array('m4v', 'mp4', 'ogv', 'webmv');
  $audio_extensions = array('mp3', 'm4a', 'oga', 'webma', 'wav');
  $poster_extensions = array('jpg', 'jpeg', 'png', 'gif');

  $poster = NULL;
  $videos = FALSE;
  $audio = FALSE;
  $media = array();

  $files = array();
  $extensions = array();
  $playlist = array();

  // Look through all the files provided and see what we have.
  foreach ($raw_files as $delta => $item) {
    // Get file URL.
    if (!isset($item['url'])) {
      $item['url'] = file_create_url($item['uri']);
    }

    // Get file extension.
    if (!isset($item['ext'])) {
      $fileinfo = pathinfo($item['url']);
      $item['ext'] = $fileinfo['extension'];
    }

    // Get file label.
    if (!isset($item['label'])) {
      if (empty($item['description'])) {
        $item['label'] = $item['filename'];
      }
      else {
        $item['label'] = $item['description'];
      }
    }

    // Add file into correct group.
    if (in_array($item['ext'], $video_extensions)) {
      $videos = TRUE;
      $item['type'] = 'video';
    }
    elseif (in_array($item['ext'], $audio_extensions)) {
      $audio = TRUE;
      $item['type'] = 'audio';
    }
    elseif (in_array($item['ext'], $poster_extensions)) {
      $poster = $item['url'];
      $item['type'] = 'poster';
    }

    $media[] = $item;
  }

  $num = 0;
  foreach ($media as $file) {
    if ($videos == TRUE && $type != 'playlist') {
      if ($file['type'] == 'audio') {
        unset($file);
      }
      $player_type = 'video';
    }
    elseif ($videos == TRUE && $type == 'playlist') {
      $player_type = 'video';
    }
    else {
      $player_type = 'audio';
    }

    if (isset($file) && $file['type'] == 'audio') {
      $files[][$file['ext']] = $file['url'];
      $extensions[] = $file['ext'];
      $playlist[] = l($file['label'], $file['url'], array('attributes' => array('id' => $player_id . '_item_' . $num, 'tabindex' => 1)));
      $num++;
    }
    elseif (isset($file) && $file['type'] == 'video') {
      $files[][$file['ext']] = $file['url'];
      if ($poster != NULL) {
        $files[]['poster'] = $poster;
      }
      $extensions[] = $file['ext'];
      $playlist[] = l($file['label'], $file['url'], array('attributes' => array('id' => $player_id . '_item_' . $num, 'tabindex' => 1)));
      $num++;
    }
  }

  if ($type == 'single') {
    $item = array_shift($playlist);
    $playlist = array();
    $playlist[] = $item;
  }

  return array('files' => $files, 'extensions' => implode(',', $extensions), 'playlist' => $playlist, 'type' => $player_type);
}
