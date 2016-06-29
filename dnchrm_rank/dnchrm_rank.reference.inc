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
 * Module to handle types of rank reference of human resource management
 * 
 * Handling types of rank reference to handle management of human resources
 */

function dnchrm_rank_types_of_rank_references(array $form = array(), array $form_state = array(), $filter = NULL) {
  $scheme = dnchrm_rank_content_type_schema_vars(TRUE, 'types_of_rank');
  if (empty($scheme['content_type'])) {
    unset ($scheme);
    drupal_set_message(t('Wrong scheme parameters :('), 'error', FALSE);
    return $form;
  }
  $nodetype = node_type_get_type($scheme['content_type']);
  
  if (empty($form_state['storage']['effective_date'])) {
    if (empty($_GET['filter']) && empty($filter)) {
      $form['effective_date'] = array();
      $form['structural_boolean'] = array();
      foreach ($scheme['fields'] as $keys => $values) {
        if (in_array($keys, array('effective_date', 'structural_boolean'))) {
          if ($values == 'title') {
            $form[$keys]['#title'] = $nodetype->title_label;
            $form[$keys]['#type'] = 'textfield';
            $form[$keys]['#required'] = TRUE;
            $form[$keys]['#size'] = 40;
          }
          else {
            $instance = field_info_instance('node', $values, $scheme['content_type']);
            $form[$keys]['#title'] = $instance['label'];
            $form[$keys]['#required'] = $instance['required'];
            $form[$keys]['#description'] = $instance['description'];
            $finfo = field_info_field($values);
            if ($instance['widget']['type'] == 'date_popup') {
              $form[$keys]['#type'] = 'date_popup';
              foreach ($instance['widget']['settings'] as $ky => $vl) {
                if (!empty($vl)) {
                  $ky = '#date_' . preg_replace('/^input_/', '', $ky);
                  if ($ky == '#date_format') {
                    $vl = dncsystem_granularity_to_date_format($finfo['settings']['granularity']);
                  }
                  $form[$keys][$ky] = $vl;
                }
              }
            }
            elseif($instance['widget']['type'] == 'options_onoff') {
              $form[$keys]['#type'] = 'checkbox';
              $form[$keys]['#title'] = $instance['label'] . ' ?!';
            }
            elseif (preg_match('/^list/', $finfo['type'])) {
              $form[$keys]['#type'] = 'select';
              $form[$keys]['#options'] = $finfo['settings']['allowed_values'];
            }
            elseif ($finfo['type'] == 'entityreference') {
              $options = array();
              $conditions = array();
              foreach ($finfo['settings']['handler_settings']['target_bundles'] as $vl) {
                if (!empty($vl)) {
                  $conditions['type'][] = $vl;
                }
              }
              $dump = entity_load($finfo['settings']['target_type'], FALSE, array('type' => $conditions));
              unset ($conditions);
              if (!empty($dump)) {
                foreach ($dump as $node) {
                  $options[$node->nid] = $node->title;
                }
              }
              unset ($dump);
              $form[$keys]['#type'] = 'select';
              $form[$keys]['#options'] = $options;
              unset ($options);
            }
            else {
              $form[$keys]['#type'] = 'textfield';
              $form[$keys]['#size'] = empty($instance['widget']['settings']['size']) ? 40 : $instance['widget']['settings']['size'];
              $form[$keys]['#maxlength'] = empty($finfo['settings']['max_length']) ? 255 : $finfo['settings']['max_length'];
            }
            unset ($finfo, $instance);
          }
        }
      }
      $form['setdate'] = array(
        '#type' => 'submit',
        '#value' => t('set'),
        '#submit' => array('dnchrm_rank_types_of_rank_references_set_date'),
        '#weight' => 100,
      );
      return $form;
    }
    else {
      if (empty($filter)) {
        if (empty($_GET['filter'])) {
          drupal_set_message(t('No effective date info available :('), 'error', FALSE);
          return $form;
        }
        $filter = $_GET['filter'];
      }
      $filter = dncsystem_unserialize(base64_decode(urldecode($filter)));
      $checkdate = empty($filter['effective_date']) ? FALSE : dncsystem_check_invalid_date($filter['effective_date']);
      if (empty($filter['effective_date']) || empty($checkdate)) {
        unset ($filter);
        drupal_set_message(t('No effective date info available :('), 'error', FALSE);
        return $form;
      }
      unset ($checkdate);
      $form_state['storage']['effective_date'] = $filter['effective_date'];
      if (!empty($filter['structural_boolean'])) {
        $form_state['storage']['structural_boolean'] = $filter['structural_boolean'];
      }
      unset ($filter);
    }
  }
  
  return dnchrm_rank_types_of_rank_references_form($form, $form_state);
}

/**
 * Submit action of reference effective data choosing
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_rank_types_of_rank_references_set_date(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['rebuild'] = TRUE;
  $scheme = dnchrm_rank_content_type_schema_vars(TRUE, 'types_of_rank');
  foreach ($form_state['values'] as $keys => $values) {
    if (!empty($values) && !empty($scheme['fields'][$keys])) {
      $form_state['storage'][$keys] = $values;
    }
  }
}

/**
 * Execution action of types of rank reference management
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_rank_types_of_rank_references_submit(&$form, &$form_state) {
  dnchrm_rank_types_of_rank_references_form_submit($form, $form_state);
  $filter = array(
    'effective_date' => $form_state['values']['effective_date'],
  );
  if (!empty($form_state['values']['structural_boolean'])) {
    $filter['structural_boolean'] = $form_state['values']['structural_boolean'];
  }
  $filter = urlencode(base64_encode(serialize($filter)));
  $form_state['redirect'] = array(current_path(), array(
    'query' => array(
      'filter' => $filter
    )
  ));
  unset ($filter);
  drupal_set_message(t('Types of rank have been saved :)'));
}

