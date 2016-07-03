<?php

/**
 * @file
 * Display the structure of organization in the system
 * 
 * All configuration processing will be handled
 * by this module
 */

/**
 * Displaying organization structure
 * 
 * @param array $form
 * @param array $form_state
 * 
 * @return string|array|int
 *   form array to display structure
 */
function dnchrm_position_structure(array $form = array(), array $form_state = array()) {
  $scheme = dnchrm_position_content_type_schema_vars(TRUE, 'positions_of_employee');
  if (empty($scheme['content_type'])) {
    unset ($scheme);
    drupal_set_message(t('System scheme empty :('), 'error', FALSE);
    return $form;
  }
  if (empty($form_state['storage']['effective_date'])) {
    if (empty($_GET['effective_date'])) {
      $form['effective_date'] = array(
        '#title' => t('Effective Date'),
        '#required' => TRUE,
      );
      if (module_exists('date_popup')) {
        $form['effective_date']['#type'] = 'date_popup';
        $form['effective_date']['#date_format'] = 'Y-m-d';
        $form['effective_date']['#date_year_range'] = '-60:0';
        $form['effective_date']['#date_label_position'] = 'none';
      }
      else {
        $form['effective_date']['#type'] = 'textfield';
        $form['effective_date']['#size'] = 20;
      }
      unset ($scheme);
      $form['setdate'] = array(
        '#type' => 'submit',
        '#value' => t('Set Date'),
        '#submit' => array('dnchrm_position_structure_set_date'),
        '#weight' => 100,
      );
      return $form;
    }
    $form_state['storage']['effective_date'] = check_plain($_GET['effective_date']);
  }
  
  $employees = dnchrm_position_get_employees($form_state['storage']['effective_date']);
  if (!empty($employees)) {
    foreach ($employees as $keys => $values) {
      $position = dncsystem_content_get_data('dnchrm_position', 'positions', $values['position']);
      if (!empty($position[$values['position']])) {
        $position = $position[$values['position']];
        $positiondate = $position['effective_date'];
        break;
      }
      unset ($position);
    }
  }
  if (empty($positiondate)) {
    $positiondate = dnchrm_position_get_positions_max_date();
  }
  
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();
  dnchrm_set_modal_settings('large');
  $editlink = ctools_modal_text_button(
    t('Manage Position'),
    'dnchrm-structure/nojs/form/setposition/' . $positiondate,
    t('Manage Position'),
    'ctools-modal-dnchrm-modal-style'
  );
  $form['addposition'] = array(
    '#type' => 'item',
    '#title' => '<strong>+</strong>',
    '#markup' => $editlink,
    '#weight' => -10,
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  unset ($editlink);
  
  $data = dnchrm_position_get_positions($positiondate);
  if (empty($data)) {
    unset ($data, $effective_date);
    return $form;
  }
  $levels = array();
  $max = 0;
  $before = 0;
  foreach ($data as $keys => $values) {
    if ($values['level'] > $max) {
      $before = $max;
      $max = $values['level'];
    }
    $parent = empty($values['parent_position']) ? 0 : $values['parent_position'];
    $levels[$values['level']][$keys] = $values;
    unset ($parent);
  }
  $deep = $before;
  unset ($before, $max);
  $ldump = array();
  $pbefore = array();
  foreach ($levels as $keys => $values) {
    $dsort = array();
    $dumpl = array();
    foreach ($values as $key => $value) {
      $value['parent_position'] = empty($value['parent_position']) ? 0 : $value['parent_position'];
      $dumpl[$key] = $value;
      $dsort[$value['parent_position']][$key] = empty($value['weight']) ? 0 : $value['weight'];
    }
    if (!empty($pbefore[$keys - 1])) {
      foreach($pbefore[$keys - 1] as $key => $value) {
        foreach ($value as $k => $v) {
          if (!empty($dsort[$v])) {
            if (count($dsort[$v]) > 1) {
              $ddump = $dsort[$v];
              asort($ddump);
              $dsort[$v] = $ddump;
              unset ($ddump);
            }
            foreach ($dsort[$v] as $ky => $vl) {
              $ldump[$keys][$v][] = $dumpl[$ky];
              $pbefore[$keys][$dumpl[$ky]['parent_position']][] = $dumpl[$ky]['nid'];
            }
          }
          else {
            $ldump[$keys][$v][] = NULL;
          }
        }
      }
      unset ($pbefore[$keys - 1]);
    }
    else {
      foreach ($dumpl as $key => $value) {
        $ldump[$keys][$key][] = $value;
        $pbefore[$keys][$value['parent_position']][] = $value['nid'];
      }
    }
    unset ($beforep, $dsort, $dumpl, $checkp);
  }
  $levels = $ldump;
  unset ($pbefore, $ldump);
  $allchilds = array();
  foreach ($levels[$deep] as $values) {
    foreach ($values as $key => $value) {
      $allchilds[] = $value;
    }
  }
  $table = array();
  $table['header'] = array(
    array(
      'data' => t('Organization Structure'),
      'colspan' => count($allchilds),
      'style' => 'text-align: center',
    ),
  );
  unset ($allchilds);
  $table['rows'] = array();
  foreach ($levels as $keys => $values) {
    $rows = array();
    foreach ($values as $key => $value) {
      foreach ($value as $ky => $vl) {
        $colspan = 0;
        if ($vl['level'] < $deep) {
          $children = array();
          dnchrm_position_get_children($vl['nid'], $deep, $children);
          if (!empty($children)) {
            foreach ($children as $dump) {
              foreach ($dump as $k => $v) {
                $colspan++;
              }
            }
          }
          unset ($children);
        }
        $officials = dnchrm_position_get_employees($form_state['storage']['effective_date'], $vl['nid']);
        $title = empty($officials) || count($officials) < $vl['count'] ? l($vl['title'], 'dnchrm/structure/' . $form_state['storage']['effective_date'] . '/' . $vl['nid']) : $vl['title'];
        if (!empty($officials)) {
          $employees = array();
          $u = 0;
          foreach ($officials as $val) {
            $dump = dncsystem_content_get_data('dnchrm', 'persons', $val['person'], NULL, 'full');
            $dump = $dump[$val['person']];
            $employees[$u] = array(
              'ofnid' => $val['nid'],
              'nid' => $dump['nid'],
              'title' => $dump['title'],
              'thumbnail' => image_style_url($dump['v_picture']['#image_style'], $dump['v_picture']['#item']['uri']),
              'picture' => $dump['v_picture']['#path']['path'],
            );
            module_load_include('inc', 'dnchrm', 'dnchrm.register');
            $dump = dnchrm_register_get_by_person($val['person']);
            if (!empty($dump)) {
              $employees[$u]['register'] = $dump;
            }
            $u++;
            unset ($dump);
          }
          unset ($u);
//          echo '<pre>'; print_r($employees); echo '</pre>'; exit;
          if (count($employees) > 1) {
            $title .= '<br /><ul>';
          }
            foreach ($employees as $val) {
              $title .= count($employees) > 1 ? '<li>' : '<br />';
              $title .= dncsystem_circle_image($val['thumbnail'], $val['picture']);
              $etitle = $val['title'] . ' (' . $val['register'] . ')';
              $etitle = l($etitle, 'node/' . $val['ofnid'], array(
                'attributes' => array(
                  'target' => '_blank'
                )
              ));
              $title .= (count($employees) < 2 ? '<br />' : NULL) . $etitle;
              unset ($etitle);
              if (count($employees) > 1) {
                $title .= '</li>';
              }
              else {
                break;
              }
            }
          if (count($employees) > 1) {
            $title .= '</ul>';
          }
          unset ($employees);
        }
        $rows[] = array(
          'data' => '<strong>' . $title . '</strong>',
          'style' => 'text-align: center; vertical-align: top',
          'colspan' => $colspan,
        );
        unset ($colspan, $title);
      }
    }
    $table['rows'][] = $rows;
    unset ($rows);
  }
  $form['effectivedate'] = array(
    '#type' => 'item',
    '#title' => t('Effective Date'),
    '#markup' => dncsystem_date('l, j F Y', strtotime($form_state['storage']['effective_date'])),
    '#weight' => 1,
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $form['ctable'] = array(
    '#type' => 'item',
    '#markup' => theme('table', $table),
    '#weight' => 10,
  );
  unset ($table);
  return $form;
}

/**
 * Submit form to setting date to display structure
 * 
 * @param type $form
 * @param type $form_state
 */
function dnchrm_position_structure_set_date(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['rebuild'] = TRUE;
  $form_state['storage']['effective_date'] = $form_state['values']['effective_date'];
}

/**
 * Form to allocate officials of the available structure
 * 
 * @param string $effectivedate
 *   string date e.g. '2016-01-01'
 * @param int $position
 *   NID of the position
 * 
 * @return array
 *   form array
 */
function dnchrm_position_structure_allocate($effectivedate = NULL, $position = NULL) {
  if (empty($effectivedate) || empty($position)) {
    drupal_set_message(t('No effective date and position info :('), 'error', FALSE);
    return;
  }
  if (!user_access('use dnc hrm')) {
    drupal_set_message(t('Sorry you unauthorized! :('), 'error', FALSE);
    return;
  }
  $scheme = dnchrm_position_content_type_schema_vars(TRUE, 'positions_of_employee');
  if (empty($scheme['content_type'])) {
    unset($scheme);
    return;
  }
  $pdata = dncsystem_content_get_data('dnchrm_position', 'positions', $position, NULL, 'full');
  if (empty($pdata[$position])) {
    unset ($pdata, $scheme);
    drupal_set_message(t('No position like this :('), 'error', FALSE);
    return;
  }
  $pdata = $pdata[$position];
  $official = dnchrm_position_get_employees($effectivedate, $position);
  if (!empty($official) && count($official) >= $pdata['count']) {
    unset ($official, $pdata, $scheme);
    drupal_set_message(t('Quota overloaded! :('), 'error', FALSE);
    return;
  }
  $cnt = empty($official) ? 1 : count($official) + 1;
  $cnt = str_repeat('0', 5 - strlen($cnt)) . $cnt;
  unset ($official);
  
  module_load_include('inc', 'node', 'node.pages');
  $user = user_load(1);
  $_GET['destination'] = 'dnchrm/structure/?effective_date=' . $effectivedate;
  $node = (object) array(
    'type' => $scheme['content_type'],
    'language' => LANGUAGE_NONE,
    'title' => $pdata['the_length'] . ' (' . $effectivedate . ' ' . $cnt . ')',
  );
  unset ($cnt);
  node_object_prepare($node);
  $node->uid = $user->uid;
  $node->name = $user->name;
  unset ($user);
  foreach ($scheme['fields'] as $keys => $values) {
    if (in_array($keys, array('effective_date', 'position'))) {
      $usedval = $keys == 'effective_date' ? $effectivedate : $position;
      if ($values == 'title') {
        $node->title = $usedval;
      }
      else {
        $finfo = field_info_field($values);
        $column = $finfo['type'] == 'entityreference' ? 'target_id' : 'value';
        $node->{$values}[$node->language][0][$column] = $usedval;
        if ($finfo['type'] == 'entityreference') {
          $node->{$values}[$node->language][0]['target_type'] = $finfo['settings']['target_type'];
        }
        unset ($finfo, $column);
      }
      unset ($usedval);
    }
  }
  $types = node_type_get_types();
  drupal_set_title(t('Create @name', array('@name' => $types[$scheme['content_type']]->name)), PASS_THROUGH);
  $form = drupal_get_form($scheme['content_type'] . '_node_form', $node);
  unset ($node, $scheme, $types, $pdata);
  return drupal_render($form);
}

