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

  $form['options']['jplayer_solution'] = array(
    '#type' => 'select',
    '#title' => t('Preferred solution'),
    '#options' => array(
      'html, flash' => t('HTML5'),
      'flash, html' => t('Flash'),
    ),
    '#default_value' => variable_get('jplayer_solution', 'html, flash'),
  );

  $form['options']['jplayer_preload'] = array(
    '#type' => 'select',
    '#title' => t('Preload media'),
    '#options' => array(
      'metadata' => t('No'),
      'auto' => t('Yes'),
    ),
    '#description' => t("Preloading media before it's requested isn't available in all browsers."),
    '#default_value' => variable_get('jplayer_preload', 'metadata'),
  );

  $form['options']['jplayer_volume'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial volume'),
    '#field_suffix' => '%',
    '#maxlength' => 3,
    '#size' => 3,
    '#default_value' => variable_get('jplayer_volume', 80),
  );

  $form['options']['jplayer_muted'] = array(
    '#type' => 'select',
    '#title' => t('Initially muted'),
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_muted', FALSE),
  );

  $form['options']['jplayer_repeat'] = array(
    '#type' => 'select',
    '#title' => t('Repeat'),
    '#options' => array(
      'all' => t('All'),
      'single' => t('Single'),
      'none' => t('None'),
    ),
    '#default_value' => variable_get('jplayer_repeat', 'none'),
  );

  $form['options']['jplayer_background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Background color'),
    '#field_prefix' => '#',
    '#maxlength' => 6,
    '#size' => 6,
    '#default_value' => variable_get('jplayer_background_color', '000000'),
  );

  $form['options']['jplayer_autoplay'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto-play files on page load'),
    '#description' => t('Use caution when combining this option with multiple players on the same page.'),
    '#default_value' => variable_get('jplayer_autoplay', ''),
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

  $form['timeformat'] = array(
    '#type' => 'fieldset',
    '#title' => t('Time Format'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['timeformat']['jplayer_showHour'] = array(
    '#title' => t('Display hours'),
    '#type' => 'select',
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_showHour', FALSE),
  );

  $form['timeformat']['jplayer_showMin'] = array(
    '#title' => t('Display minutes'),
    '#type' => 'select',
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_showMin', TRUE),
  );

  $form['timeformat']['jplayer_showSec'] = array(
    '#title' => t('Display seconds'),
    '#type' => 'select',
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_showSec', TRUE),
  );

  $form['timeformat']['jplayer_padHour'] = array(
    '#title' => t('Zero-pad the hours'),
    '#type' => 'select',
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_padHour', FALSE),
  );

  $form['timeformat']['jplayer_padMin'] = array(
    '#title' => t('Zero-pad the minutes'),
    '#type' => 'select',
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_padMin', TRUE),
  );

  $form['timeformat']['jplayer_padSec'] = array(
    '#title' => t('Zero-pad the seconds'),
    '#type' => 'select',
    '#options' => array(
      FALSE => t('No'),
      TRUE => t('Yes'),
    ),
    '#default_value' => variable_get('jplayer_padSec', TRUE),
  );

  $form['timeformat']['jplayer_sepHour'] = array(
    '#title' => t('Hours seperator'),
    '#type' => 'textfield',
    '#maxlength' => 32,
    '#default_value' => variable_get('jplayer_sepHour', ':'),
  );

  $form['timeformat']['jplayer_sepMin'] = array(
    '#title' => t('Minutes seperator'),
    '#type' => 'textfield',
    '#maxlength' => 32,
    '#default_value' => variable_get('jplayer_sepMin', ':'),
  );

  $form['timeformat']['jplayer_sepSec'] = array(
    '#title' => t('Seconds seperator'),
    '#type' => 'textfield',
    '#maxlength' => 32,
    '#default_value' => variable_get('jplayer_sepSec', ''),
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

  $volume = $form_state['values']['jplayer_volume'];
  if ($volume < 0 || $volume > 100) {
    form_error($form['options']['jplayer_volume'], t('Volume must be a percentage between 0% and 100%.'));
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

