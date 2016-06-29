<?php

/**
 * @file
 * Handling administration action in the system
 * 
 * All configuration processing will be handled
 * by this module
 */

/**
 * Configure Family data of HRM system parameters
 * 
 * @param array $form
 *   variable arry of form
 * @param array $form_state
 *   variable array of form_state
 * 
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dnchrm_family_admin_params(array $form = array(), array $form_state = array()) {
  $info = dnchrm_family_content_type_schema_vars(FALSE, NULL, TRUE);
  $form['content types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Types'),
    '#description' => t('Main content type that be used in this system'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 0,
  );
  $form['content types']['scheme'] = array(
    '#markup' => $info,
  );
  unset ($info);
  
  return $form;
}

/**
 * Adding new person's mate data
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person pass via path args
 * 
 * @return array
 *   form array that will be render by drupal_get_form
 */
function dnchrm_family_mate_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['module'] = 'dnchrm_family';
  $form_state['storage']['context'] = 'mates';
  $form_state['storage']['checkedfields'] = array('gender', 'date_of_birth');
  $form_state['storage']['destination'] = 'dnchrm/family';
  return dnchrm_build_add_item_form($form, $form_state);
}

/**
 * Listing and manage person's mate and the children
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * @param integer $mate
 *   NID of the mate
 * 
 * @return array|int
 *   form array ready to rendering to listing table format
 */
function dnchrm_family_mate(array $form = array(), array $form_state = array(), $person, $mate) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  if (empty($form_state['storage']['mate'])) {
    if (empty($mate)) {
      return $form;
    }
    $form_state['storage']['mate'] = $mate;
  }
  
  foreach ($form_state['storage'] as $keys => $values) {
    $form[$keys] = array(
      '#type' => 'value',
      '#value' => $values,
    );
  }
  $data = dncsystem_content_get_data('dnchrm_family', 'mates', $mate, NULL, 'full');
  $data = empty($data[$mate]) ? array() : $data[$mate];
  $form['infos'] = array(
    '#type' => 'fieldset',
    '#title' => t('<strong>@mate</strong>\'s info', array('@mate' => empty($data['title']) ? t('Mate') : $data['title'])),
    '#description' => t('Information about the person\'s mate'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -10,
  );
  $display = node_load($form_state['storage']['mate']);
  $display = node_view($display);
  $form['infos']['mate'] = array(
    '#type' => 'item',
    '#markup' => drupal_render($display),
    '#weight' => 10,
  );
  unset ($display);
  if (user_access('use dnc hrm')) {
    $form['infos']['mateupdate'] = array(
      '#type' => 'submit',
      '#value' => t('update'),
      '#submit' => array('dnchrm_family_update_mate'),
      '#weight' => 20,
      '#prefix' => '<div class="container-inline">',
    );
    $form['infos']['matedelete'] = array(
      '#type' => 'submit',
      '#value' => t('delete'),
      '#submit' => array('dnchrm_family_delete_mate'),
      '#weight' => 21,
      '#suffix' => '</div">',
    );
  }
  
  $children = dnchrm_family_get_data_by_person_and_mate(
          $form_state['storage']['person'],
          $form_state['storage']['mate'],
          'dnchrm_family',
          'childrens'
         );
  if (empty($children)) {
    unset ($children);
    return $form;
  }
//  echo '<pre>'; print_r($children); echo '</pre>'; exit;
  $form['childrensshow'] = array(
    '#type' => 'item',
    '#markup' => t('<strong>@mate</strong> has <strong>@count</strong> children as below...', array(
      '@mate' => $data['title'],
      '@count' => number_format(count($children), 0, ',', '.')
    )),
  );
  unset ($data, $children);
  $form_state['storage']['datatitle'] = 'Family';
  $form_state['storage']['context'] = 'childrens';
  $form_state['storage']['module'] = 'dnchrm_family';
  $form_state['storage']['drupalrender'] = 'teaser';
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Implementing hook_submit for listing manage mate's children above
 * 
 * @param array $form
 * @param array $form_state
 * 
 * @return string
 *   execute and redirecting to the current path
 */
function dnchrm_family_mate_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  if (!user_access('use dnc hrm')) {
    drupal_set_message(t('Sorry you\'re not authorized :('), 'error', FALSE);
    return;
  }
  if (empty($form_state['input']['items']) || empty($form_state['values']['person'])) {
    return;
  }
  $scheme = dnchrm_family_content_type_schema_vars(TRUE, 'childrens');
  foreach ($form_state['input']['items'] as $keys => $values) {
    if (!empty($values['delete'])) {
      dncsystem_content_delete('dnchrm_family', 'childrens', $keys);
    }
  }
  unset ($scheme);
  drupal_set_message(t('Update childrens have been done successfully :)'));
}

/**
 * Handling custom validating to update mate
 * before it be forwarded to default drupal node update form
 * 
 * @param array $form
 * @param array $form_state
 * 
 * @return string
 *   redirecting to node update
 */
function dnchrm_family_update_mate(&$form, &$form_state) {
  form_state_values_clean($form_state);
  drupal_goto('node/' . $form_state['values']['mate'] . '/edit', array(
    'query' => array(
      'destination' => 'dnchrm/family/' . $form_state['values']['person'] . '/' . $form_state['values']['mate']
    ),
  ));
  return;
}

/**
 * Handling custom submit forwarding to delete mate
 * before it be forwarded to default drupal node delete form
 * 
 * @param array $form
 * @param array $form_state
 * 
 * @return string
 *   redirecting to node delete
 */
function dnchrm_family_delete_mate(&$form, &$form_state) {
  form_state_values_clean($form_state);
  drupal_goto('node/' . $form_state['values']['mate'] . '/delete', array(
    'query' => array(
      'destination' => 'dnchrm/family'
    ),
  ));
  return;
}

/**
 * Generating custom action to add children
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * @param integer $mate
 *   NID of the person's mate
 * 
 * @return array
 *   form array that ready to render by durpal node form
 */
function dnchrm_family_child_add(array $form = array(), array $form_state = array(), $person, $mate) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  if (empty($form_state['storage']['mate'])) {
    if (empty($mate)) {
      return $form;
    }
    $form_state['storage']['mate'] = $mate;
  }
  
  $form_state['storage']['module'] = 'dnchrm_family';
  $form_state['storage']['context'] = 'childrens';
  $form_state['storage']['includefields'] = array('mate' => $form_state['storage']['mate']);
  $form_state['storage']['checkedfields'] = array('gender', 'date_of_birth');
  $form_state['storage']['destination'] = 'dnchrm/family/' . $form_state['storage']['person'] . '/' . $form_state['storage']['mate'];
  $data = dncsystem_content_get_data('dnchrm_family', 'mates', $mate, 'title');
  $data = empty($data[$mate]['title']) ? t('mate') : $data[$mate]['title'];
  $form_state['storage']['pagetitle'] = t('Add child of @mate', array('@mate' => $data));
  unset ($data);
  
  return dnchrm_build_add_item_form($form, $form_state);
}

