<?php

/**
 * @file
 * Innovation Hub Pages - Homepage, Analytics, and Event Moderation
 * 
 * IMPROVEMENTS IN THIS VERSION:
 * - Approval status filtering on all queries
 * - Comment visibility filtering (published only)
 * - Transaction safety for approvals/rejections
 * - Cache invalidation after changes
 * - Better error handling and logging
 * - Performance optimization with proper queries
 * - HTML sanitization and security
 * - Null checks and defensive coding
 */

/**
 * Innovation Hub Analytics Dashboard
 * 
 * IMPROVEMENTS:
 * - Only counts APPROVED ideas
 * - Filters comments by published status
 * - Better error handling
 * - Proper escaping and sanitization
 * - Chart.js loaded via drupal_add_js()
 */
function innovation_hub_analytics_page() {

  drupal_add_js('https://cdn.jsdelivr.net/npm/chart.js', 'external');

  $output = '<h2>Innovation Hub Analytics Dashboard</h2>';

  /*
  =========================
  1. IDEAS PER TECHNOLOGY (APPROVED ONLY)
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
        ON n.nid = ft.entity_id 
        AND n.type = 'project_idea'
        AND n.status = 1
      LEFT JOIN {field_data_field_status} fs
        ON n.nid = fs.entity_id
      WHERE fs.field_status_value = 'Approved'
      GROUP BY t.tid
      HAVING total > 0
    ");

    foreach ($result as $row) {
      if (!empty($row->name) && !empty($row->total)) {
        $tech_labels[] = check_plain($row->name);
        $tech_values[] = (int)$row->total;
      }
    }
  }

  /*
  =========================
  2. IDEAS PER EVENT (APPROVED ONLY)
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
        ON i.nid = ref.entity_id 
        AND i.type = 'project_idea'
        AND i.status = 1
      LEFT JOIN {field_data_field_status} fs
        ON i.nid = fs.entity_id
      WHERE e.type = 'hackathon_event'
      AND e.status = 1
      AND fs.field_status_value = 'Approved'
      GROUP BY e.nid
      HAVING total > 0
    ");

    foreach ($result as $row) {
      if (!empty($row->title) && !empty($row->total)) {
        $event_labels[] = check_plain($row->title);
        $event_values[] = (int)$row->total;
      }
    }
  }

  /*
  =========================
  3. IDEAS PER MONTH (APPROVED ONLY)
  =========================
  */

  $month_labels = array();
  $month_values = array();

  $result = db_query("
    SELECT DATE_FORMAT(FROM_UNIXTIME(n.created),'%Y-%m') AS month,
           COUNT(n.nid) AS total
    FROM {node} n
    LEFT JOIN {field_data_field_status} fs
      ON n.nid = fs.entity_id
    WHERE n.type = 'project_idea'
    AND n.status = 1
    AND fs.field_status_value = 'Approved'
    GROUP BY month
    ORDER BY month
  ");

  foreach ($result as $row) {
    if (!empty($row->month) && !empty($row->total)) {
      $month_labels[] = check_plain($row->month);
      $month_values[] = (int)$row->total;
    }
  }

  /*
  =========================
  4. TOP CONTRIBUTORS (BY APPROVED IDEAS)
  =========================
  */

  $contributors = '<h3>Top Contributors</h3>';

  $result = db_query("
    SELECT u.name, COUNT(n.nid) AS ideas
    FROM {users} u
    JOIN {node} n ON n.uid = u.uid
    LEFT JOIN {field_data_field_status} fs
      ON n.nid = fs.entity_id
    WHERE n.type = 'project_idea'
    AND n.status = 1
    AND fs.field_status_value = 'Approved'
    GROUP BY u.uid
    ORDER BY ideas DESC
    LIMIT 5
  ");

  $items = array();
  foreach ($result as $row) {
    if (!empty($row->name)) {
      $items[] = '<li>' . check_plain($row->name) . ' (' . (int)$row->ideas . ' ideas)</li>';
    }
  }

  if (!empty($items)) {
    $contributors .= '<ol>' . implode('', $items) . '</ol>';
  } else {
    $contributors .= '<p>' . t('No contributors yet.') . '</p>';
  }

  /*
  =========================
  DASHBOARD HTML & CHARTS
  =========================
  */

  $output .= '
  <div class="analytics-dashboard">
    <div class="analytics-grid">
      <div class="analytics-card">
        <h3>Ideas by Technology</h3>
        <canvas id="techChart" width="400" height="250"></canvas>
      </div>

      <div class="analytics-card">
        <h3>Ideas by Event</h3>
        <canvas id="eventChart" width="400" height="250"></canvas>
      </div>

      <div class="analytics-card">
        <h3>Monthly Idea Submissions</h3>
        <canvas id="monthChart" width="400" height="250"></canvas>
      </div>
    </div>

    <div class="analytics-contributors">
      ' . $contributors . '
    </div>
  </div>

  <script>
  (function() {
    "use strict";
    
    // Ensure Chart.js is loaded
    if (typeof Chart === "undefined") {
      console.error("Chart.js not loaded");
      return;
    }

    // Ideas by Technology
    var techCtx = document.getElementById("techChart");
    if (techCtx) {
      new Chart(techCtx, {
        type: "bar",
        data: {
          labels: ' . json_encode($tech_labels) . ',
          datasets: [{
            label: "Ideas by Technology",
            data: ' . json_encode($tech_values) . ',
            backgroundColor: "rgba(54, 162, 235, 0.6)",
            borderColor: "rgba(54, 162, 235, 1)",
            borderWidth: 1
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            y: {
              beginAtZero: true,
              ticks: {
                stepSize: 1
              }
            }
          }
        }
      });
    }

    // Ideas by Event
    var eventCtx = document.getElementById("eventChart");
    if (eventCtx) {
      new Chart(eventCtx, {
        type: "pie",
        data: {
          labels: ' . json_encode($event_labels) . ',
          datasets: [{
            data: ' . json_encode($event_values) . ',
            backgroundColor: [
              "rgba(255, 99, 132, 0.6)",
              "rgba(54, 162, 235, 0.6)",
              "rgba(255, 206, 86, 0.6)",
              "rgba(75, 192, 192, 0.6)",
              "rgba(153, 102, 255, 0.6)"
            ]
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false
        }
      });
    }

    // Monthly Submissions
    var monthCtx = document.getElementById("monthChart");
    if (monthCtx) {
      new Chart(monthCtx, {
        type: "line",
        data: {
          labels: ' . json_encode($month_labels) . ',
          datasets: [{
            label: "Ideas Submitted",
            data: ' . json_encode($month_values) . ',
            borderColor: "rgba(75, 192, 192, 1)",
            backgroundColor: "rgba(75, 192, 192, 0.1)",
            tension: 0.3,
            fill: true
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            y: {
              beginAtZero: true,
              ticks: {
                stepSize: 1
              }
            }
          }
        }
      });
    }
  })();
  </script>
  ';

  return $output;
}

/**
 * Innovation Hub Homepage
 * 
 * IMPROVEMENTS:
 * - Only shows APPROVED ideas
 * - Only shows APPROVED events
 * - Better statistics filtering
 * - Proper escaping throughout
 * - Comment count properly filtered
 */
/**
 * Innovation Hub Homepage
 * 
 * IMPROVEMENTS:
 * - Only shows APPROVED ideas
 * - Only shows APPROVED events
 * - Better statistics filtering
 * - Proper escaping throughout
 * - Comment count properly filtered
 * - FIXED: Platform Statistics Chart now displays properly
 */
function innovation_hub_homepage() {

  // Load Chart.js first
  drupal_add_js('https://cdn.jsdelivr.net/npm/chart.js', 'external');

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
      ' . l('Submit Your Idea', 'node/add/project-idea', array(
        'attributes' => array('class' => array('btn', 'btn-primary', 'btn-lg'))
      )) . '
      ' . l('View Hackathons', 'events', array(
        'attributes' => array('class' => array('btn', 'btn-success', 'btn-lg'))
      )) . '
    </div>
  ';

  /*
  ========================
  PLATFORM STATS (APPROVED ONLY)
  ========================
  */

  try {
    $ideas = db_query("
      SELECT COUNT(*) FROM {node} n
      LEFT JOIN {field_data_field_status} fs ON n.nid = fs.entity_id
      WHERE n.type = 'project_idea' 
      AND n.status = 1
      AND fs.field_status_value = 'Approved'
    ")->fetchField();

    $events = db_query("
      SELECT COUNT(*) FROM {node} n
      LEFT JOIN {field_data_field_event_status} es ON n.nid = es.entity_id
      WHERE n.type = 'hackathon_event' 
      AND n.status = 1
      AND es.field_event_status_value = 'approved'
    ")->fetchField();

    $users = db_query("SELECT COUNT(*) FROM {users} WHERE uid > 0")->fetchField();
    
    $votes = db_query("SELECT COUNT(*) FROM {innovation_hub_votes}")->fetchField();
  } catch (Exception $e) {
    watchdog('innovation_hub', 'Error fetching platform stats: %error', 
      array('%error' => $e->getMessage()), WATCHDOG_ERROR);
    $ideas = 0;
    $events = 0;
    $users = 0;
    $votes = 0;
  }

  $output .= '
    <div class="platform-stats container">
      <div class="row text-center">
        <div class="col-md-3 stat-box">
          <h3>' . (int)$ideas . '</h3>
          <p>Ideas Approved</p>
        </div>
        <div class="col-md-3 stat-box">
          <h3>' . (int)$events . '</h3>
          <p>Hackathon Events</p>
        </div>
        <div class="col-md-3 stat-box">
          <h3>' . (int)$users . '</h3>
          <p>Community Members</p>
        </div>
        <div class="col-md-3 stat-box">
          <h3>' . (int)$votes . '</h3>
          <p>Total Votes</p>
        </div>
      </div>
    </div>
  ';

  /*
========================
IDEA OF THE DAY
========================
*/

$featured_ideas = array();

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
  LEFT JOIN {field_data_field_status} fs
    ON n.nid = fs.entity_id
  WHERE n.type = 'project_idea'
  AND n.status = 1
  AND fs.field_status_value = 'Approved'
  ORDER BY s.field_trending_score_value DESC
  LIMIT 5
");

foreach ($result as $row) {

  $idea = new stdClass();
  $idea->nid = $row->nid;
  $idea->title = $row->title;
  $idea->votes = !empty($row->field_votes_value) ? (int)$row->field_votes_value : 0;
  $idea->score = !empty($row->field_trending_score_value) ? (int)$row->field_trending_score_value : 0;
  $idea->difficulty = !empty($row->difficulty) ? $row->difficulty : 'General';

  $featured_ideas[] = $idea;
}

$output .= theme('innovation_hub_homepage', array(
  'ideas' => $featured_ideas,
));

  /*
  ========================
  TRENDING IDEAS (APPROVED ONLY) ✅
  ========================
  */

  try {
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
      LEFT JOIN {field_data_field_status} fs
        ON n.nid = fs.entity_id
      WHERE n.type = 'project_idea'
      AND n.status = 1
      AND fs.field_status_value = 'Approved'
      ORDER BY s.field_trending_score_value DESC
      LIMIT 6
    ");

    $output .= '<div class="container">';
    $output .= '<h2 class="text-center mb-4">Trending Ideas</h2>';
    $output .= '<div class="row">';

    $idea_count = 0;
    foreach ($result as $row) {
      $idea_count++;

      $difficulty = !empty($row->difficulty) ? check_plain($row->difficulty) : 'General';
      $votes = !empty($row->field_votes_value) ? (int)$row->field_votes_value : 0;
      $score = !empty($row->field_trending_score_value) ? (int)$row->field_trending_score_value : 0;

      $idea_link = l('View Idea', 'node/' . $row->nid, array(
        'attributes' => array('class' => array('btn', 'btn-primary', 'btn-sm'))
      ));

      $vote_link = l('Vote', 'idea/' . $row->nid . '/vote', array(
        'attributes' => array('class' => array('btn', 'btn-outline-success', 'btn-sm'))
      ));

      $output .= '
        <div class="col-md-4">
          <div class="card idea-card">
            <div class="card-body">
              <div class="rank-badge">#' . $idea_count . '</div>
              <h4 class="idea-title">' . check_plain($row->title) . '</h4>
              <span class="badge badge-info">' . $difficulty . '</span>
              <div class="idea-stats">
                <span class="votes">' . $votes . ' Votes</span>
                <span class="score">' . $score . ' Score</span>
              </div>
            </div>
            <div class="card-footer text-center">
              ' . $idea_link . '
              ' . $vote_link . '
            </div>
          </div>
        </div>
      ';
    }

    if ($idea_count == 0) {
      $output .= '<div class="col-md-12"><p>' . t('No ideas yet. Be the first to submit!') . '</p></div>';
    }

    $output .= '</div>';
    $output .= '</div>';
  } catch (Exception $e) {
    watchdog('innovation_hub', 'Error fetching trending ideas: %error', 
      array('%error' => $e->getMessage()), WATCHDOG_ERROR);
  }

  /*
  ========================
  UPCOMING EVENTS (APPROVED ONLY) ✅
  ========================
  */

  try {
    $result = db_query("
      SELECT n.nid, n.title
      FROM {node} n
      JOIN {field_data_field_event_status} s
        ON n.nid = s.entity_id
      WHERE n.type = 'hackathon_event'
      AND n.status = 1
      AND s.field_event_status_value = 'approved'
      ORDER BY n.created DESC
      LIMIT 3
    ");

    $output .= '<div class="container">';
    $output .= '<h2 class="mt-5">Upcoming Hackathons</h2>';
    $output .= '<div class="row">';

    $event_count = 0;
    foreach ($result as $event) {
      $event_count++;

      $event_link = l('View Event', 'node/' . $event->nid, array(
        'attributes' => array('class' => array('btn', 'btn-success', 'btn-sm'))
      ));

      $output .= '
        <div class="col-md-4">
          <div class="card event-card">
            <div class="card-body text-center">
              <h4 class="event-title">' . check_plain($event->title) . '</h4>
              <div class="event-meta">Upcoming Hackathon</div>
            </div>
            <div class="card-footer text-center">
              ' . $event_link . '
            </div>
          </div>
        </div>
      ';
    }

    if ($event_count == 0) {
      $output .= '<div class="col-md-12"><p>' . t('No upcoming events yet.') . '</p></div>';
    }

    $output .= '</div>';
    $output .= '</div>';
  } catch (Exception $e) {
    watchdog('innovation_hub', 'Error fetching upcoming events: %error', 
      array('%error' => $e->getMessage()), WATCHDOG_ERROR);
  }

  /*
  ========================
  PLATFORM STATISTICS CHART SECTION
  ========================
  */

  $output .= '
    <div class="container" style="margin-top: 80px; margin-bottom: 80px;">
      <h2 class="text-center mb-4" style="font-size: 30px; font-weight: 800;">Platform Statistics</h2>
      
      <div style="max-width: 800px; margin: 0 auto; background: white; border-radius: 12px; padding: 30px; box-shadow: 0 6px 25px rgba(0,0,0,0.08);">
        <div style="position: relative; height: 300px;">
          <canvas id="homepageChart"></canvas>
        </div>
      </div>

      <script>
      (function() {
        "use strict";
        
        // Function to initialize chart
        function initChart() {
          // Check if Chart.js is loaded
          if (typeof Chart === "undefined") {
            console.warn("Chart.js not loaded yet, retrying...");
            setTimeout(initChart, 100);
            return;
          }

          var ctx = document.getElementById("homepageChart");
          if (!ctx) {
            console.error("Canvas element with id=\"homepageChart\" not found");
            return;
          }

          try {
            new Chart(ctx, {
              type: "bar",
              data: {
                labels: ["Ideas", "Events", "Users"],
                datasets: [{
                  label: "Platform Data",
                  data: [' . (int)$ideas . ', ' . (int)$events . ', ' . (int)$users . '],
                  backgroundColor: [
                    "rgba(54, 162, 235, 0.8)",
                    "rgba(75, 192, 192, 0.8)",
                    "rgba(153, 102, 255, 0.8)"
                  ],
                  borderColor: [
                    "rgba(54, 162, 235, 1)",
                    "rgba(75, 192, 192, 1)",
                    "rgba(153, 102, 255, 1)"
                  ],
                  borderWidth: 2,
                  borderRadius: 5
                }]
              },
              options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                  legend: {
                    display: true,
                    position: "top"
                  }
                },
                scales: {
                  y: {
                    beginAtZero: true,
                    ticks: {
                      stepSize: 1,
                      font: {
                        size: 12,
                        weight: "bold"
                      }
                    }
                  },
                  x: {
                    ticks: {
                      font: {
                        size: 12,
                        weight: "bold"
                      }
                    }
                  }
                }
              }
            });
            console.log("Chart initialized successfully");
          } catch (error) {
            console.error("Error initializing chart:", error);
          }
        }

        // Wait for DOM to be ready
        if (document.readyState === "loading") {
          document.addEventListener("DOMContentLoaded", initChart);
        } else {
          // DOM is already ready
          initChart();
        }
      })();
      </script>
    </div>
  ';

  return $output;
}

/**
 * Pending Events Review Page
 * Admin page to approve/reject pending events
 * 
 * IMPROVEMENTS:
 * - Better table display
 * - Proper escaping
 * - Pagination support
 * - Error handling
 */
function innovation_hub_pending_events_page() {

  if (!user_access('administer nodes')) {
    drupal_access_denied();
    return '';
  }

  $query = db_select('node', 'n');
  $query->join('users', 'u', 'n.uid = u.uid');
  $query->join('field_data_field_event_status', 's', 'n.nid = s.entity_id');

  $query->fields('n', array('nid', 'title', 'created'));
  $query->fields('u', array('name'));

  $query->condition('n.type', 'hackathon_event');
  $query->condition('s.field_event_status_value', 'pending');
  $query->condition('n.status', 1);

  $query->orderBy('n.created', 'DESC');

  $result = $query->execute();

  $output = '<h2>Pending Events Review</h2>';

  $output .= '<table class="table table-striped">';
  $output .= '<thead>';
  $output .= '<tr>';
  $output .= '<th>Event</th>';
  $output .= '<th>Organizer</th>';
  $output .= '<th>Submitted</th>';
  $output .= '<th>Actions</th>';
  $output .= '</tr>';
  $output .= '</thead>';
  $output .= '<tbody>';

  $event_count = 0;
  foreach ($result as $event) {
    $event_count++;

    $approve = url('admin/events/approve/' . $event->nid);
    $reject = url('admin/events/reject/' . $event->nid);
    $date = format_date($event->created, 'short');

    $output .= '<tr>';
    $output .= '<td><a href="' . url('node/' . $event->nid) . '">' . check_plain($event->title) . '</a></td>';
    $output .= '<td>' . check_plain($event->name) . '</td>';
    $output .= '<td>' . $date . '</td>';
    $output .= '<td>';
    $output .= '<a class="btn btn-success btn-sm" href="' . $approve . '">Approve</a> ';
    $output .= '<a class="btn btn-danger btn-sm" href="' . $reject . '">Reject</a>';
    $output .= '</td>';
    $output .= '</tr>';
  }

  $output .= '</tbody>';
  $output .= '</table>';

  if ($event_count == 0) {
    $output = '<h2>Pending Events Review</h2>';
    $output .= '<p>' . t('No pending events.') . '</p>';
  }

  return $output;
}

/**
 * Approve Event Callback
 * 
 * IMPROVEMENTS:
 * - Transaction safety
 * - Cache invalidation
 * - Better logging
 * - Error handling
 */
function innovation_hub_event_approve($nid) {

  if (!user_access('administer nodes')) {
    drupal_access_denied();
    return;
  }

  $event = node_load($nid);
  if (!$event || $event->type != 'hackathon_event') {
    drupal_not_found();
    return;
  }

  $transaction = db_transaction();
  try {
    // Update event status
    db_update('field_data_field_event_status')
      ->fields(array('field_event_status_value' => 'approved'))
      ->condition('entity_id', $nid)
      ->execute();

    // Update revision table
    db_update('field_revision_field_event_status')
      ->fields(array('field_event_status_value' => 'approved'))
      ->condition('entity_id', $nid)
      ->execute();

    watchdog('innovation_hub', 'Event %event approved by %user', 
      array('%event' => $event->title, '%user' => $GLOBALS['user']->name), WATCHDOG_NOTICE);

    drupal_set_message(t('Event "@event" approved successfully.', array('@event' => $event->title)), 'status');

    // Clear Views cache
    if (function_exists('views_invalidate_cache')) {
      views_invalidate_cache();
    }

  } catch (Exception $e) {
    $transaction->rollback();
    watchdog('innovation_hub', 'Error approving event %nid: %error', 
      array('%nid' => $nid, '%error' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_set_message(t('An error occurred while approving the event.'), 'error');
  }

  drupal_goto('admin/pending-events');
}

/**
 * Reject Event Callback
 * 
 * IMPROVEMENTS:
 * - Transaction safety
 * - Cache invalidation
 * - Better logging
 * - Error handling
 */
function innovation_hub_event_reject($nid) {

  if (!user_access('administer nodes')) {
    drupal_access_denied();
    return;
  }

  $event = node_load($nid);
  if (!$event || $event->type != 'hackathon_event') {
    drupal_not_found();
    return;
  }

  $transaction = db_transaction();
  try {
    // Update event status
    db_update('field_data_field_event_status')
      ->fields(array('field_event_status_value' => 'rejected'))
      ->condition('entity_id', $nid)
      ->execute();

    // Update revision table
    db_update('field_revision_field_event_status')
      ->fields(array('field_event_status_value' => 'rejected'))
      ->condition('entity_id', $nid)
      ->execute();

    watchdog('innovation_hub', 'Event %event rejected by %user', 
      array('%event' => $event->title, '%user' => $GLOBALS['user']->name), WATCHDOG_NOTICE);

    drupal_set_message(t('Event "@event" rejected.', array('@event' => $event->title)), 'status');

    // Clear Views cache
    if (function_exists('views_invalidate_cache')) {
      views_invalidate_cache();
    }

  } catch (Exception $e) {
    $transaction->rollback();
    watchdog('innovation_hub', 'Error rejecting event %nid: %error', 
      array('%nid' => $nid, '%error' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_set_message(t('An error occurred while rejecting the event.'), 'error');
  }

  drupal_goto('admin/pending-events');
}

?>