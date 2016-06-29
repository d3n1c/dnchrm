<?php

/* 
 * Copyright (C) 2016 denic wibowo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * Module to handle academic title of human resource management
 * 
 * Handling academic title data to handle management of human resources
 */

/**
 * Custom form to manage certificate of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_education_academic_title (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $title = dnchrm_get_data_by_person($form_state['storage']['person'], 'dnchrm_education', 'academic_titles');
  if (!empty($title)) {
    $pname = dnchrm_education_person_title($form_state['storage']['person']);
    $form['ptitle'] = array(
      '#type' => 'item',
      '#title' => t('Title Influed') . ': ',
      '#markup' => ' => <strong>' . $pname . '</strong>',
      '#weight' => -50,
      '#prefix' => '<div class="container-inline">',
      '#suffix' => '</div>',
    );
    unset ($pname);
  }
  unset ($title);
  
  $form_state['storage']['datatitle'] = 'Educations';
  $form_state['storage']['context'] = 'academic_titles';
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['drupalrender'] = 'teaser';
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Custom form to add academic title education
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_education_academic_title_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  if (empty($form_state['storage']['includefields']['formal'])) {
    if (empty($_GET['formal'])) {
      drupal_set_message(t('No formal education data :('), 'error', FALSE);
      return $form;
    }
    $form_state['storage']['includefields'] = array('formal' => check_plain($_GET['formal']));
  }
  
  $form_state['storage']['context'] = 'academic_titles';
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['destination'] = 'dnchrm/education/' . $form_state['storage']['person'] . '/academic-titles';
  return dnchrm_build_add_item_form($form, $form_state);
}

/**
 * Validating form to asking new person's title name
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * @param integer $formal
 *   NID of the person's formal education
 * 
 * @return array
 *   form array that ready to render by drupal builtin node add form
 */
function dnchrm_education_academic_ask_title(array $form = array(), array $form_state = array(), $person = NULL, $formal = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      drupal_set_message(t('No person data :('), 'error', FALSE);
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  if (empty($form_state['storage']['formal'])) {
    if (empty($formal)) {
      drupal_set_message(t('No formal education data :('), 'error', FALSE);
      return $form;
    }
    $form_state['storage']['formal'] = check_plain($formal);
  }
  if (!empty($form_state['storage'])) {
    foreach ($form_state['storage'] as $keys => $values) {
      $form[$keys] = array(
        '#type' => 'value',
        '#value' => $values
      );
    }
  }
  $description = t('May be with the new information of formal education of the person, you want to add new title regarding this education, e.g. Dr, dr., SE., M.Sc. etc.');
  return confirm_form(
          $form,
          t('Are you want to add person\'s name title'),
          'dnchrm/education/' . $form_state['storage']['person'] . '/formals',
          $description,
          t('Yes add one please'),
          t('No, back to main list')
        );
}

/**
 * Implementing hook_submit to validation form above
 * It will redirect to the correct adding title form
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_education_academic_ask_title_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = array('dnchrm/education/' . $form_state['values']['person'] . '/academic-titles/add', array(
    'query' => array(
      'formal' => $form_state['values']['formal']
    )
  ));
}
