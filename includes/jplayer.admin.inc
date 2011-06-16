<?php

/**
 * @file
 * Administrative pages for the jPlayer module.
 */

/**
 * Menu callback; Provides the jPlayer settings form.
 */
function jplayer_settings_form() {
  $form = array();

  $form['jplayer_directory'] = array(
    '#type' => 'textfield',
    '#title' => t('jPlayer file directory'),
    '#default_value' => variable_get('jplayer_directory', 'sites/all/libraries/jplayer'),
    '#description' => t('Specify the path that contains the jPlayer library. The jplayer.player.min.js file should be in the root of this directory.'),
  );

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('jPlayer options'),
    '#collapsible' => FALSE,
  );

  $form['options']['jplayer_autoplay'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto-play files on page load'),
    '#description' => t('Use caution when combining this option with multiple players on the same page.'),
    '#default_value' => variable_get('jplayer_autoplay', ''),
  );

  $form['options']['jplayer_pause_others'] = array(
    '#type' => 'checkbox',
    '#title' => t('Pause other jPlayers'),
    '#description' => t('Prevents multiple jPlayers from playing at once on a single page. If a second player is started, the current player is paused.'),
    '#default_value' => variable_get('jplayer_pause_others', FALSE),
  );

  $form['options']['jplayer_protected'] = array(
    '#title' => t('Protect audio files from direct downloads'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('jplayer_protected', FALSE),
  );

  if (variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC) == FILE_DOWNLOADS_PUBLIC) {
    $form['options']['jplayer_protected']['#description'] = t('To enable file download protection, first <a href="@file-system-settings">set the site download method to Private</a>.', array('@file-system-settings' => url('admin/settings/file-system', array('query' => drupal_get_destination()))));
  }

  $form['options']['jplayer_access_time'] = array(
    '#title' => t('Access delay'),
    '#type' => 'textfield',
    '#default_value' => variable_get('jplayer_access_time', 30),
    '#size' => 5,
    '#description' => t('The number of seconds that a client will have access to download a protected file after it is requested by jPlayer.'),
  );

  $form = system_settings_form($form);
  $form['#validate'][] = 'jplayer_settings_form_validate';
  $form['#submit'][] = 'jplayer_settings_form_submit';
  return $form;
}

/**
 * Validation function to validate the jplayer_settings_form() form.
 */
function jplayer_settings_form_validate($form, &$form_state) {
  $form_state['jplayer_version'] = jplayer_get_version($form_state['values']['jplayer_directory']);
  if (!$form_state['jplayer_version']) {
    form_error($form['jplayer_directory'], t('The directory specified does not seem to contain the jPlayer library. Check to make sure that the jquery.player.min.js file is located within this directory.'));
  }

  $time = $form_state['values']['jplayer_access_time'];
  if ($form_state['values']['jplayer_protected'] && !is_numeric($time)) {
    form_error($form['options']['jplayer_access_time'], t('Access time must be a value in seconds.'));
  }
  if (intval($time) < 0) {
    form_error($form['options']['jplayer_access_time'], t('Access time must be at least 0 seconds.'));
  }
}

/**
 * Submit handler for the jplayer_settings_form() form.
 */
function jplayer_settings_form_submit($form, &$form_state) {
  drupal_set_message(t('The jPlayer library (version @version) successfully found in the %directory directory.', array('@version' => $form_state['jplayer_version'], '%directory' => $form_state['values']['jplayer_directory'])));
}

/**
 * Page callback for the jPlayer protection statistics page. This page is
 * useful in determining if a browser is misbehaving and blocking legitimiate
 * file accesses, or if a user is trying to download a protected file.
 */
function jplayer_protection_statistics() {
  if (!variable_get('jplayer_protected', FALSE)) {
    drupal_set_message(t('<a href="@jplayer-settings">jPlayer content protection</a> is not currently enabled.', array('@jplayer-settings' => url('admin/settings/jplayer', array('query' => drupal_get_destination())))));
  }

  $output = '<p>' . t('This table shows the 50 top users who have been denied access to direct downloads of jPlayer files.') . '</p>';

  $result = db_query("SELECT COUNT(1) as total, uid as user, MAX(timestamp) as timestamp FROM {jplayer_denied} GROUP BY uid ORDER BY total DESC, timestamp DESC LIMIT 50;");
  $rows = array();
  while ($denied = db_fetch_array($result)) {
    // Format data from the query.
    $uid = $denied['user'];
    $denied['user'] = theme('username', user_load($denied['user']));
    $denied['timestamp'] = format_date($denied['timestamp']);

    // Find the top-denied file for the user.
    $top_file = db_fetch_object(db_query("SELECT COUNT(fid) as fid_count, fid FROM {jplayer_denied} WHERE uid = %d GROUP BY fid ORDER BY fid_count DESC LIMIT 1", $uid));
    $top_file = db_fetch_object(db_query("SELECT * FROM {files} WHERE fid = %d", $top_file->fid));
    $denied['file'] = $top_file->filepath;

    // Find the top hostname for the user.
    $top_hostname = db_fetch_object(db_query("SELECT COUNT(hostname) as hostname_count, hostname FROM {jplayer_denied} WHERE uid = %d GROUP BY hostname ORDER BY hostname_count DESC LIMIT 1", $uid));
    $denied['hostname'] = $top_hostname->hostname;

    $rows[] = $denied;
  }

  $header = array(
    t('Accesses denied'),
    t('User'),
    t('Last denied'),
    t('Top Denied File'),
    t('Top Hostname'),
  );

  if (!empty($rows)) {
    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= '<p>' . t('There have been no files that have been denied access within the last week.') . '</p>';
  }
  return $output;
}

