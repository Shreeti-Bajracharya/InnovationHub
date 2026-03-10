<?php

function innovation_hub_analytics_page() {

  $output = '<h2>Innovation Hub Analytics Dashboard</h2>';

  /*
  =========================
  1. IDEAS PER TECHNOLOGY
  =========================
  */

  $tech_labels = array();
  $tech_values = array();

  if (db_table_exists('field_data_field_technology_tags')) {

    $result = db_query("
      SELECT t.name, COUNT(n.nid) AS total
      FROM {taxonomy_term_data} t
      LEFT JOIN {field_data_field_technology_tags} ft
        ON t.tid = ft.field_technology_tags_tid
      LEFT JOIN {node} n
        ON n.nid = ft.entity_id AND n.type = 'project_idea'
      GROUP BY t.tid
    ");

    foreach ($result as $row) {
      $tech_labels[] = $row->name;
      $tech_values[] = $row->total;
    }
  }


  /*
  =========================
  2. IDEAS PER EVENT
  =========================
  */

  $event_labels = array();
  $event_values = array();

  if (db_table_exists('field_data_field_related_event')) {

    $result = db_query("
      SELECT e.title, COUNT(i.nid) AS total
      FROM {node} e
      LEFT JOIN {field_data_field_related_event} ref
        ON ref.field_related_event_target_id = e.nid
      LEFT JOIN {node} i
        ON i.nid = ref.entity_id AND i.type='project_idea'
      WHERE e.type='hackathon_event'
      GROUP BY e.nid
    ");

    foreach ($result as $row) {
      $event_labels[] = $row->title;
      $event_values[] = $row->total;
    }
  }


  /*
  =========================
  3. IDEAS PER MONTH
  =========================
  */

  $month_labels = array();
  $month_values = array();

  $result = db_query("
    SELECT DATE_FORMAT(FROM_UNIXTIME(created),'%Y-%m') AS month,
           COUNT(nid) AS total
    FROM {node}
    WHERE type='project_idea'
    GROUP BY month
    ORDER BY month
  ");

  foreach ($result as $row) {
    $month_labels[] = $row->month;
    $month_values[] = $row->total;
  }


  /*
  =========================
  4. TOP CONTRIBUTORS
  =========================
  */

  $contributors = '<h3>Top Contributors</h3><ol>';

  $result = db_query("
    SELECT u.name, COUNT(n.nid) AS ideas
    FROM {users} u
    JOIN {node} n ON n.uid = u.uid
    WHERE n.type='project_idea'
    GROUP BY u.uid
    ORDER BY ideas DESC
    LIMIT 5
  ");

  foreach ($result as $row) {
    $contributors .= '<li>'.check_plain($row->name).' ('.$row->ideas.' ideas)</li>';
  }

  $contributors .= '</ol>';


  /*
  =========================
  DASHBOARD HTML
  =========================
  */

  $output .= '

  <h3>Ideas by Technology</h3>
  <canvas id="techChart"></canvas>

  <h3>Ideas by Event</h3>
  <canvas id="eventChart"></canvas>

  <h3>Monthly Idea Submissions</h3>
  <canvas id="monthChart"></canvas>

  '.$contributors.'

  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

  <script>

  new Chart(document.getElementById("techChart"), {
    type:"bar",
    data:{
      labels:'.json_encode($tech_labels).',
      datasets:[{
        label:"Ideas",
        data:'.json_encode($tech_values).'
      }]
    }
  });

  new Chart(document.getElementById("eventChart"), {
    type:"pie",
    data:{
      labels:'.json_encode($event_labels).',
      datasets:[{
        data:'.json_encode($event_values).'
      }]
    }
  });

  new Chart(document.getElementById("monthChart"), {
    type:"line",
    data:{
      labels:'.json_encode($month_labels).',
      datasets:[{
        label:"Ideas",
        data:'.json_encode($month_values).'
      }]
    }
  });

  </script>
  ';

  return $output;
}

function innovation_hub_homepage() {

  $output = '';

  /*
  ========================
  HERO SECTION
  ========================
  */

  $output .= '
<div class="jumbotron text-center hero-banner">
<h1>Welcome to Innovation Hub</h1>
<p>Submit your ideas and join hackathons</p>

'.l('Submit Your Idea', 'node/add/project-idea', array(
'attributes' => array('class' => array('btn','btn-primary','btn-lg'))
)).'

'.l('View Hackathons', 'events', array(
'attributes' => array('class' => array('btn','btn-success','btn-lg'))
)).'

</div>
';
    
/*
========================
TRENDING IDEAS
========================
*/

$result = db_query("
  SELECT n.nid, n.title,
v.field_votes_value,
s.field_trending_score_value,
t.name AS difficulty
FROM {node} n
LEFT JOIN {field_data_field_votes} v
  ON n.nid = v.entity_id
LEFT JOIN {field_data_field_trending_score} s
  ON n.nid = s.entity_id
LEFT JOIN {field_data_field_difficulty} d
  ON n.nid = d.entity_id
LEFT JOIN {taxonomy_term_data} t
  ON d.field_difficulty_tid = t.tid
WHERE n.type='project_idea'
AND n.status = 1
ORDER BY s.field_trending_score_value DESC
LIMIT 6
");

$output .= '<div class="container">';
$output .= '<h2 class="text-center mb-4">Trending Ideas</h2>';
$output .= '<div class="row">';

foreach ($result as $row) {

  $difficulty = !empty($row->difficulty)
  ? check_plain($row->difficulty)
  : 'General';

  $idea_link = l('View Idea', 'node/'.$row->nid, array(
    'attributes' => array('class'=>array('btn','btn-primary','btn-sm'))
  ));

  $vote_link = l('Vote', 'idea/'.$row->nid.'/vote', array(
    'attributes' => array('class'=>array('btn','btn-outline-success','btn-sm'))
  ));

  $votes = !empty($row->field_votes_value) ? $row->field_votes_value : 0;
  $score = !empty($row->field_trending_score_value) ? $row->field_trending_score_value : 0;

  $output .= '

  <div class="col-md-4">

    <div class="card idea-card">

      <div class="card-body">

        <h4 class="idea-title">'.check_plain($row->title).'</h4>

        <span class="badge badge-info">'.$difficulty.'</span>

        <div class="idea-stats">

          <span class="votes">
            '.$votes.' Votes
          </span>

          <span class="score">
            '.$score.' Score
          </span>

        </div>

      </div>

      <div class="card-footer text-center">

        '.$idea_link.'
        '.$vote_link.'

      </div>

    </div>

  </div>

  ';
}

$output .= '</div>';
/*
========================
UPCOMING EVENTS
========================
*/

$result = db_query("
SELECT n.nid, n.title
FROM {node} n
JOIN {field_data_field_event_status} s
ON n.nid = s.entity_id
WHERE n.type='hackathon_event'
AND s.field_event_status_value='approved'
ORDER BY n.created DESC
LIMIT 3
");

$output .= '<h2 class="mt-5">Upcoming Hackathons</h2>';
$output .= '<div class="row">';

foreach ($result as $event) {

  $event_link = l('View Event', 'node/'.$event->nid, array(
    'attributes' => array(
      'class' => array('btn','btn-success','btn-sm')
    )
  ));

  $output .= '

  <div class="col-md-4">

    <div class="card event-card">

      <div class="card-body">

        <h4>'.check_plain($event->title).'</h4>

        '.$event_link.'

      </div>

    </div>

  </div>

  ';
}

$output .= '</div>';

/*
========================
ANALYTICS PREVIEW
========================
*/

$output .= '

<h2 class="mt-5">Platform Statistics</h2>

<canvas id="homepageChart"></canvas>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<script>

new Chart(document.getElementById("homepageChart"), {

type:"bar",

data:{
labels:["Ideas","Events","Users"],

datasets:[{
label:"Platform Data",
data:[
'.db_query("SELECT COUNT(*) FROM {node} WHERE type='project_idea'")->fetchField().',
'.db_query("SELECT COUNT(*) FROM {node} WHERE type='hackathon_event'")->fetchField().',
'.db_query("SELECT COUNT(*) FROM {users}")->fetchField().'
]
}]
}

});

</script>

';

$output .= '</div>';

return $output;

}

function innovation_hub_pending_events_page() {

  $query = db_select('node', 'n');
  $query->join('users', 'u', 'n.uid = u.uid');
  $query->join('field_data_field_event_status', 's', 'n.nid = s.entity_id');

  $query->fields('n', array('nid', 'title', 'created'));
  $query->fields('u', array('name'));

  $query->condition('n.type', 'hackathon_event');
  $query->condition('s.field_event_status_value', 'pending');

  $query->orderBy('n.created', 'DESC');

  $result = $query->execute();

  $output = '<h2>Pending Events</h2>';

  $output .= '<table class="table table-striped">';
  $output .= '<thead>';
  $output .= '<tr>';
  $output .= '<th>Event</th>';
  $output .= '<th>Organizer</th>';
  $output .= '<th>Date</th>';
  $output .= '<th>Actions</th>';
  $output .= '</tr>';
  $output .= '</thead>';
  $output .= '<tbody>';

  foreach ($result as $event) {

    $approve = url('admin/events/approve/' . $event->nid);
    $reject = url('admin/events/reject/' . $event->nid);

    $date = format_date($event->created, 'short');

    $output .= '<tr>';
    $output .= '<td><a href="'.url('node/'.$event->nid).'">'.check_plain($event->title).'</a></td>';
    $output .= '<td>'.check_plain($event->name).'</td>';
    $output .= '<td>'.$date.'</td>';
    $output .= '<td>';
    $output .= '<a class="btn btn-success btn-sm" href="'.$approve.'">Approve</a> ';
    $output .= '<a class="btn btn-danger btn-sm" href="'.$reject.'">Reject</a>';
    $output .= '</td>';
    $output .= '</tr>';

  }

  $output .= '</tbody>';
  $output .= '</table>';

  return $output;
}

function innovation_hub_event_approve($nid) {

  if (!user_access('administer nodes')) {
  drupal_access_denied();
}

  db_update('field_data_field_event_status')
    ->fields(array(
      'field_event_status_value' => 'approved'
    ))
    ->condition('entity_id', $nid)
    ->execute();

  watchdog('innovation_hub', 'Event approved.');

  drupal_set_message('Event approved.');

  drupal_goto('admin/pending-events');
}

function innovation_hub_event_reject($nid) {

  if (!user_access('administer nodes')) {
  drupal_access_denied();
}

  db_update('field_data_field_event_status')
    ->fields(array(
      'field_event_status_value' => 'rejected'
    ))
    ->condition('entity_id', $nid)
    ->execute();

  watchdog('innovation_hub', 'Event rejected.');

  drupal_set_message('Event rejected.');

  drupal_goto('admin/pending-events');
}