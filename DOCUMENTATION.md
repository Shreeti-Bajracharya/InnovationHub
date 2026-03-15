# InnovationHub – Complete Beginner-Friendly Documentation

**Author:** Shreeti Bajracharya  
**Platform:** Drupal 7.95 (PHP / MySQL)  
**Purpose:** Hackathon collaboration platform for submitting ideas, voting, and managing events

---

## Table of Contents

1. [What Is This Project?](#1-what-is-this-project)
2. [Technology Stack](#2-technology-stack)
3. [How the Repository Is Organized](#3-how-the-repository-is-organized)
4. [Drupal Core Concepts (For Beginners)](#4-drupal-core-concepts-for-beginners)
   - 4.1 What Is Drupal?
   - 4.2 Nodes (Content)
   - 4.3 Modules
   - 4.4 Hooks – The Heart of Drupal
   - 4.5 Themes
   - 4.6 Views
   - 4.7 Taxonomy
   - 4.8 Entity Reference
   - 4.9 Fields
   - 4.10 Blocks
5. [Content Types](#5-content-types)
6. [Custom Module: `innovation_hub`](#6-custom-module-innovation_hub)
   - 6.1 Module Files Overview
   - 6.2 `innovation_hub.info`
   - 6.3 `innovation_hub.module` – Function by Function
   - 6.4 `innovation_hub.pages.inc` – Page Callbacks
   - 6.5 `innovation_hub.install` – Database Setup
   - 6.6 Template: `innovation-hub-homepage.tpl.php`
7. [Database Design](#7-database-design)
8. [The Voting System](#8-the-voting-system)
9. [The Trending Algorithm](#9-the-trending-algorithm)
10. [The Analytics Dashboard](#10-the-analytics-dashboard)
11. [Admin Workflow: Event Moderation](#11-admin-workflow-event-moderation)
12. [Security Features](#12-security-features)
13. [Caching System](#13-caching-system)
14. [Contributed Modules Explained](#14-contributed-modules-explained)
15. [The Bootstrap Theme](#15-the-bootstrap-theme)
16. [Testing: SimpleTest / DrupalWebTestCase](#16-testing-simpletest--drupalwebtestcase)
17. [How Everything Connects – System Flow](#17-how-everything-connects--system-flow)
18. [Q&A Preparation – Senior Developer Questions](#18-qa-preparation--senior-developer-questions)

---

## 1. What Is This Project?

**InnovationHub** is a website where people can:

1. **Browse hackathon events** – Events posted by organizers.
2. **Submit project ideas** – Any logged-in user can post an idea.
3. **Vote for ideas** – Community members vote for the best ideas.
4. **See trending ideas** – A live ranking based on votes, comments, and age.
5. **View analytics** – Charts showing ideas by technology, event, month, and top contributors.
6. **Admin moderation** – Administrators approve or reject events before they go public.

Think of it like a cross between a suggestion box and a leaderboard for innovation.

---

## 2. Technology Stack

| Technology | What It Does in This Project |
|---|---|
| **Drupal 7.95** | The CMS (Content Management System) that powers everything |
| **PHP** | The server-side language Drupal is written in |
| **MySQL** | The database that stores all content, users, votes, etc. |
| **Apache (XAMPP)** | The web server that serves the website locally |
| **Bootstrap 3** | A CSS framework used for the front-end design (responsive layout, buttons, cards) |
| **Chart.js** | A JavaScript library that draws the analytics charts |
| **Composer** | PHP dependency manager (manages vendor libraries) |

---

## 3. How the Repository Is Organized

```
InnovationHub/
│
├── index.php                  ← Drupal's main entry point (do not edit)
├── includes/                  ← Drupal core library files
├── modules/                   ← Drupal core modules (built-in)
├── themes/                    ← Drupal core themes
├── profiles/                  ← Drupal installation profiles
├── vendor/                    ← Composer PHP libraries
│
└── sites/
    └── all/
        ├── modules/
        │   └── custom/
        │       └── innovation_hub/   ← ⭐ OUR CUSTOM MODULE (the important part)
        │           ├── innovation_hub.info
        │           ├── innovation_hub.module
        │           ├── innovation_hub.pages.inc
        │           ├── innovation_hub.install
        │           ├── innovation_hub.test
        │           └── templates/
        │               └── innovation-hub-homepage.tpl.php
        │
        └── themes/
            └── bootstrap/            ← The Bootstrap theme for the UI
```

> **Key insight:** Everything in `sites/all/modules/custom/innovation_hub/` is the code written specifically for this project. Everything else is Drupal's standard code.

---

## 4. Drupal Core Concepts (For Beginners)

### 4.1 What Is Drupal?

Drupal is a **Content Management System (CMS)** – a ready-made framework for building websites without starting from scratch. It handles user logins, database connections, routing (URLs), and much more. We extend it with custom code called **modules**.

Think of Drupal like a house that is already built. We are adding custom rooms (modules) and repainting (themes) without tearing down the whole structure.

---

### 4.2 Nodes (Content)

In Drupal, everything that is "content" is called a **node**. A blog post is a node. A product is a node. In InnovationHub:

- A **Project Idea** = a node of type `project_idea`
- A **Hackathon Event** = a node of type `hackathon_event`

Every node has:
- A **Node ID (nid)** – a unique number
- A **title**
- An **owner (uid)** – the user who created it
- A **status** (1 = published, 0 = unpublished)
- A **created** timestamp (Unix time)

When we write `node_load($nid)` in code, we are asking Drupal to load all the data for a specific piece of content.

---

### 4.3 Modules

A **module** is a folder with PHP code that adds new features to Drupal. Drupal includes many modules built-in (core). Additional ones called **contributed modules** can be downloaded. We also wrote our own **custom module** (`innovation_hub`).

Every module needs at minimum:
1. A `.info` file (tells Drupal the module exists and its metadata)
2. A `.module` file (the main PHP code)

---

### 4.4 Hooks – The Heart of Drupal

**Hooks are the most important concept in Drupal.** A hook is a way for your module to "hook into" Drupal's events and add custom behavior.

#### How Hooks Work (Simple Explanation)

Drupal, at certain moments, calls a function named `[module_name]_[hook_name]()`. If your module defines that function, Drupal calls it automatically.

**Real-world analogy:** Imagine a hotel that announces "Housekeeping, please clean room 5." Any housekeeper who is listening responds. Drupal "announces" events, and any module with the matching function responds.

#### Hooks Used in This Project

| Hook | Where | What It Does |
|---|---|---|
| `hook_menu()` | `innovation_hub_menu()` | Registers URL routes like `/home`, `/analytics`, `/idea/{id}/vote` |
| `hook_block_info()` | `innovation_hub_block_info()` | Tells Drupal we have a block called "Trending Ideas" |
| `hook_block_view()` | `innovation_hub_block_view()` | Provides the HTML content of the Trending Ideas block |
| `hook_node_insert()` | `innovation_hub_node_insert()` | Fires when a new node is saved; sets default "Pending" status |
| `hook_node_update()` | `innovation_hub_node_update()` | Fires when a node is edited; recalculates trending score |
| `hook_node_view()` | `innovation_hub_node_view()` | Fires when a node is displayed; adds the Vote button |
| `hook_comment_insert()` | `innovation_hub_comment_insert()` | Fires when a comment is added; recalculates trending score |
| `hook_form_alter()` | `innovation_hub_form_alter()` | Modifies the idea submission form (adds placeholders, validation) |
| `hook_theme()` | `innovation_hub_theme()` | Registers the custom homepage template |
| `hook_views_query_alter()` | `innovation_hub_views_query_alter()` | Modifies Views database queries (adds trending score sorting) |
| `hook_permission()` | `innovation_hub_permission()` | Defines custom permissions like "submit ideas", "vote on ideas" |
| `hook_flush_caches()` | `innovation_hub_flush_caches()` | Tells Drupal about our custom cache bin |
| `hook_schema()` | `innovation_hub_schema()` | Defines custom database tables |
| `hook_install()` | `innovation_hub_install()` | Runs once when the module is first enabled |
| `hook_uninstall()` | `innovation_hub_uninstall()` | Cleans up when the module is removed |
| `hook_simpletest_alter()` | `innovation_hub_simpletest_alter()` | Registers test classes with Drupal's test runner |

---

### 4.5 Themes

A **theme** controls the visual appearance of the site. It uses:
- **`.tpl.php` files** – Template files (HTML mixed with PHP)
- **CSS files** – Styling
- **JavaScript files** – Interactive behavior

This project uses the **Bootstrap** theme, which applies Bootstrap 3's grid system and component styles automatically. Custom HTML is output from `innovation-hub-homepage.tpl.php`.

---

### 4.6 Views

**Views** is a contributed module that lets you create dynamic database queries through a user interface (or code) without writing raw SQL.

In InnovationHub:
- **`/events`** – A View that lists all hackathon events
- **`/ideas`** – A View that lists all project ideas

The custom module also uses `hook_views_query_alter()` to inject extra sorting (by trending score) into the Views SQL query.

---

### 4.7 Taxonomy

**Taxonomy** is Drupal's classification system. Like tags or categories. In InnovationHub:

| Vocabulary | Terms (Examples) |
|---|---|
| **Technology** | AI, Web Development, Blockchain, IoT |
| **Category** | Healthcare, Education, Environment, Fintech |
| **Difficulty** | Beginner, Intermediate, Advanced |

Each project idea can be tagged with these terms, making it easy to filter and group ideas.

---

### 4.8 Entity Reference

**Entity Reference** is a field type that links one piece of content to another. In InnovationHub, a **Project Idea** has an entity reference field (`field_related_event`) pointing to a **Hackathon Event**. This creates a one-to-many relationship:

```
One Hackathon Event → Many Project Ideas
```

This is like a foreign key relationship in a database, but managed through Drupal's field system.

---

### 4.9 Fields

In Drupal, every piece of data attached to a node is a **field**. Fields are stored in their own database tables, named `field_data_[field_name]`. Examples from InnovationHub:

| Field Name | Database Table | What It Stores |
|---|---|---|
| `field_status` | `field_data_field_status` | Whether an idea is Pending/Approved/Rejected |
| `field_votes` | `field_data_field_votes` | Vote count for an idea |
| `field_trending_score` | `field_data_field_trending_score` | Calculated popularity score |
| `field_difficulty` | `field_data_field_difficulty` | Difficulty taxonomy term reference |
| `field_technology_tags` | `field_data_field_technology_tags` | Technology taxonomy term reference |
| `field_event_status` | `field_data_field_event_status` | Whether an event is pending/approved/rejected |
| `field_related_event` | `field_data_field_related_event` | Entity reference to a hackathon event |

---

### 4.10 Blocks

A **block** is a small, reusable region of content that can be placed in different areas of a page (sidebar, header, footer). InnovationHub defines a **"Trending Ideas" block** that shows the top 5 ideas sorted by trending score. It is registered in `hook_block_info()` and rendered in `hook_block_view()`.

---

## 5. Content Types

### 5.1 Hackathon Event (`hackathon_event`)

A hackathon event is a competition or collaborative event that organizers post on the platform.

**Fields:**
- Event Title (node title)
- Event Description (body)
- Event Date (`field_event_date`)
- Organizer (node author – the user who created it)
- Location
- Banner Image
- **Event Status** (`field_event_status`) – `pending` / `approved` / `rejected`

**Workflow:**
1. Organizer submits an event → status is `pending`
2. Admin reviews it at `/admin/pending-events`
3. Admin clicks Approve → status becomes `approved` → event appears publicly
4. Admin clicks Reject → status becomes `rejected` → event is hidden

---

### 5.2 Project Idea (`project_idea`)

A project idea is what users submit to participate in hackathons.

**Fields:**
- Idea Title (node title)
- Description (body)
- Technology Tags (`field_technology_tags`) – Taxonomy
- Category (`field_category`) – Taxonomy
- Difficulty Level (`field_difficulty`) – Taxonomy
- **Status** (`field_status`) – `Pending` / `Approved` / `Rejected`
- **Vote Count** (`field_votes`) – Synced from the votes table
- **Trending Score** (`field_trending_score`) – Calculated score
- Related Event (`field_related_event`) – Entity reference to a hackathon

**Workflow:**
1. Logged-in user submits idea → status is `Pending`
2. Admin reviews and approves → status becomes `Approved`
3. Once approved, the Vote button appears on the idea page
4. Other users can vote, which increases the trending score

---

## 6. Custom Module: `innovation_hub`

This is the core of the project — all the custom business logic lives here.

### 6.1 Module Files Overview

| File | Purpose |
|---|---|
| `innovation_hub.info` | Module metadata (name, dependencies, version) |
| `innovation_hub.module` | Main module file: hooks, voting, scoring, blocks |
| `innovation_hub.pages.inc` | Page callback functions: homepage, analytics, event moderation |
| `innovation_hub.install` | Database schema and update functions |
| `innovation_hub.test` | Automated tests (30 tests) |
| `templates/innovation-hub-homepage.tpl.php` | HTML template for "Idea of the Day" section |

---

### 6.2 `innovation_hub.info`

```
name = Innovation Hub
description = Custom module for idea voting, analytics, trending algorithm and admin dashboard
core = 7.x
dependencies[] = node
dependencies[] = views
dependencies[] = entity
dependencies[] = taxonomy
dependencies[] = comment
```

**What this does:** Tells Drupal this module exists, what it is called, which version of Drupal it works with (7.x), and which other modules it needs (dependencies).

---

### 6.3 `innovation_hub.module` – Function by Function

#### `innovation_hub_menu()` – URL Routing

```php
function innovation_hub_menu() {
  $items['home'] = array(
    'page callback' => 'innovation_hub_homepage',
    'access arguments' => array('access content'),
    'file' => 'innovation_hub.pages.inc',
  );
  // ... more routes
}
```

**What it does:** Registers these URL routes:

| URL | Handler Function | Who Can Access |
|---|---|---|
| `/home` | `innovation_hub_homepage()` | Everyone |
| `/analytics` | `innovation_hub_analytics_page()` | Everyone |
| `/idea/{nid}/vote` | `innovation_hub_vote()` | Logged-in users only |
| `/admin/pending-events` | `innovation_hub_pending_events_page()` | Admins only |
| `/admin/events/approve/{nid}` | `innovation_hub_event_approve()` | Admins only |
| `/admin/events/reject/{nid}` | `innovation_hub_event_reject()` | Admins only |

---

#### `innovation_hub_vote($node)` – The Vote Handler

This is the function that runs when a user clicks the Vote button.

**Security checks (in order):**

1. **Node type check** – Must be a `project_idea`
2. **Approval check** – Idea must have `field_status = 'Approved'`
3. **Self-vote prevention** – `$node->uid == $user->uid` → blocked
4. **Duplicate vote prevention** – Checks `innovation_hub_votes` table
5. **Rate limiting** – Max 10 votes per hour per user

**What happens on a valid vote:**

```
1. INSERT into innovation_hub_votes (uid, nid, created)
2. Count total votes for this idea
3. UPDATE field_data_field_votes with the new total
4. UPDATE field_revision_field_votes (for revision history)
5. Recalculate the trending score
6. Clear the cache
7. Show success message and redirect
```

All database operations are wrapped in a **transaction** (`db_transaction()`). If any step fails, all changes are rolled back to keep data consistent.

---

#### `innovation_hub_check_vote_rate_limit($uid)` – Rate Limiting

```php
function innovation_hub_check_vote_rate_limit($uid) {
  $one_hour_ago = REQUEST_TIME - 3600;
  $count = db_query("SELECT COUNT(*) FROM {innovation_hub_votes}
    WHERE uid = :uid AND created > :time",
    array(':uid' => $uid, ':time' => $one_hour_ago))->fetchField();
  return $count >= 10;
}
```

**What it does:** Counts how many votes the user has made in the last hour. If 10 or more, the vote is blocked.

Uses the **cache table** (`cache_innovation_hub`) to store this count temporarily, so the database is not queried on every vote attempt. Falls back to a direct query if the cache table doesn't exist yet.

---

#### `innovation_hub_block_info()` and `innovation_hub_block_view()` – Trending Block

```php
function innovation_hub_block_info() {
  $blocks['trending_ideas'] = array(
    'info' => t('Trending Ideas'),
    'cache' => DRUPAL_CACHE_GLOBAL,
  );
  return $blocks;
}
```

**What it does:** Registers a block that can be placed in any region of the site (e.g. sidebar). The block shows the top 5 approved ideas sorted by trending score. `DRUPAL_CACHE_GLOBAL` means the block HTML is cached globally (same for all users) for performance.

---

#### `innovation_hub_node_insert($node)` – Default Status on Creation

```php
function innovation_hub_node_insert($node) {
  if ($node->type == 'project_idea') {
    db_update('field_data_field_status')
      ->fields(array('field_status_value' => 'Pending'))
      ->condition('entity_id', $node->nid)
      ->execute();
    innovation_hub_update_score($node->nid);
  }
}
```

**What it does:** When a new idea is saved, automatically sets its status to "Pending" (waiting for admin approval) and initializes its trending score to 0.

---

#### `innovation_hub_node_view($node, $view_mode, $langcode)` – Vote Button

```php
function innovation_hub_node_view($node, $view_mode, $langcode) {
  if ($node->type == 'project_idea' && $is_approved) {
    $vote_link = l('👍 Vote', 'idea/'.$node->nid.'/vote', ...);
    $node->content['vote_button'] = array('#markup' => $vote_link);
  }
}
```

**What it does:** When Drupal renders a project idea page, this hook adds the Vote button to the page content – but ONLY if the idea is approved.

---

#### `innovation_hub_calculate_score($nid)` – Trending Score Formula

```php
function innovation_hub_calculate_score($nid) {
  $votes   = /* vote count from field_votes */;
  $comments = /* count of published comments */;
  $days_old = (REQUEST_TIME - $created) / 86400;

  if ($days_old <= 3)       $recency_bonus = 3;
  elseif ($days_old <= 7)   $recency_bonus = 1;
  else                      $recency_bonus = 0;

  $score = ($votes * 2) + $comments + $recency_bonus;
  return $score;
}
```

See [Section 9](#9-the-trending-algorithm) for a full explanation.

---

#### `innovation_hub_form_alter()` – Customizing Forms

```php
function innovation_hub_form_alter(&$form, &$form_state, $form_id) {
  if ($form_id == 'project_idea_node_form') {
    $form['title']['#attributes']['placeholder'] = 'Example: AI Powered Disaster Prediction System';
    $form['#validate'][] = 'innovation_hub_idea_form_validate';
  }
}
```

**What it does:** Intercepts the idea submission form and adds:
- A placeholder hint in the title field
- Custom validation (title must be 5+ characters, description 20+ characters)

---

#### `innovation_hub_idea_form_validate()` – Form Validation

```php
function innovation_hub_idea_form_validate($form, &$form_state) {
  if (strlen($title) < 5) {
    form_set_error('title', 'Idea title must be at least 5 characters.');
  }
  if (strlen(trim($body_value)) < 20) {
    form_set_error('body', 'Idea description must be at least 20 characters.');
  }
}
```

**What it does:** Called during form submission. If either condition fails, the form is not submitted and an error message is shown.

---

#### `innovation_hub_permission()` – Custom Permissions

```php
function innovation_hub_permission() {
  return array(
    'administer innovation hub' => ...,
    'submit ideas' => ...,
    'vote on ideas' => ...,
  );
}
```

**What it does:** Defines custom permission labels that site administrators can assign to user roles through the Drupal admin interface.

---

### 6.4 `innovation_hub.pages.inc` – Page Callbacks

This file contains three main page functions:

#### `innovation_hub_analytics_page()` – Analytics Dashboard

Builds the analytics page at `/analytics`. It:
1. Queries the database for ideas grouped by technology tag
2. Queries ideas grouped by event
3. Queries ideas grouped by month (using `DATE_FORMAT`)
4. Queries top contributors (users with most approved ideas)
5. Outputs HTML with `<canvas>` elements for Chart.js graphs
6. Embeds JavaScript to initialize three Chart.js charts (bar, pie, line)

The data is passed from PHP to JavaScript using `json_encode()`:
```javascript
labels: <?php echo json_encode($tech_labels); ?>
```

#### `innovation_hub_homepage()` – The Homepage

Builds the homepage at `/home`. It:
1. Loads Chart.js from a CDN
2. Renders a hero banner with links to submit ideas and view events
3. Fetches platform statistics (idea count, event count, user count, vote count)
4. Queries top 5 trending approved ideas for the "Idea of the Day" section
5. Uses `theme('innovation_hub_homepage', ...)` to render the ideas via the template
6. Shows a "Trending Ideas" section with 6 idea cards
7. Shows "Upcoming Hackathons" section
8. Shows a platform statistics bar chart

#### `innovation_hub_pending_events_page()` – Admin Event Review

Renders the page at `/admin/pending-events` showing all events with `field_event_status = 'pending'` in a table with Approve/Reject buttons. Only accessible to users with `administer nodes` permission.

#### `innovation_hub_event_approve($nid)` and `innovation_hub_event_reject($nid)`

These functions change the event status in the database:
- **Approve:** Sets `field_event_status_value = 'approved'` in both `field_data_field_event_status` and `field_revision_field_event_status`
- **Reject:** Sets `field_event_status_value = 'rejected'`

Both use **database transactions** for safety and clear the Views cache afterward.

---

### 6.5 `innovation_hub.install` – Database Setup

This file runs when the module is installed, updated, or uninstalled.

#### `hook_schema()` – Creating Custom Tables

Defines two custom database tables:

**`innovation_hub_votes` table:**

| Column | Type | Purpose |
|---|---|---|
| `vid` | serial (auto-increment) | Primary key |
| `nid` | int | Which idea was voted on |
| `uid` | int | Who voted |
| `created` | int | When the vote happened (Unix timestamp) |

Has a **unique key** on `(uid, nid)` preventing the same user from voting twice on the same idea at the database level.

Has **indexes** on `nid`, `uid`, `created`, and `(nid, created)` for fast queries.

**`cache_innovation_hub` table:**

Standard Drupal cache table structure used to store rate-limit data and other cached values.

#### Update Functions (`hook_update_N`)

These are numbered update functions that run in order when the module is updated:

| Function | What It Does |
|---|---|
| `innovation_hub_update_8001()` | Creates the votes table if missing |
| `innovation_hub_update_8002()` | Adds database indexes to the votes table |
| `innovation_hub_update_8003()` | Sets default "Pending" status on all existing ideas/events |
| `innovation_hub_update_8004()` | Calculates and stores trending scores for all existing ideas |
| `innovation_hub_update_8005()` | Creates the cache table |
| `innovation_hub_update_8006()` | Adds performance indexes to field tables |
| `innovation_hub_update_8007()` | Verifies all indexes exist |

> **Note on numbering:** Drupal 7 conventionally names these functions with a `7xxx` series (e.g. `innovation_hub_update_7001`). This project uses `8xxx` numbering instead. Both work in Drupal 7 — Drupal simply runs any unexecuted `hook_update_N` functions in ascending numerical order. The `8xxx` numbering does not make this a Drupal 8 module; it is simply an unusual (but valid) convention choice made during development.

---

### 6.6 Template: `innovation-hub-homepage.tpl.php`

This is a **PHP template file** that generates the "Idea of the Day" HTML section.

```php
<?php if (!empty($ideas)): ?>
  <?php
    // Sort ideas by score descending
    usort($ideas, function($a, $b) {
      return $b->score - $a->score;
    });
    $featured_idea = $ideas[0]; // Take the top idea
  ?>
  <section class="trending-ideas-section">
    <h2>💡 Idea of the Day</h2>
    <h3>
      <a href="<?php print url('node/' . $featured_idea->nid); ?>">
        <?php print check_plain($featured_idea->title); ?>
      </a>
    </h3>
    <!-- votes, score, difficulty badge, buttons -->
  </section>
<?php else: ?>
  <p>No ideas submitted yet. Be the first to add one!</p>
<?php endif; ?>
```

**Key function used: `check_plain()`** – This escapes special HTML characters like `<`, `>`, `"` to prevent XSS attacks. Every piece of user-generated content is passed through `check_plain()` before being displayed.

---

## 7. Database Design

### Core Tables Used

```
[node]                        ← All content (ideas, events, articles)
  nid, vid, type, title, uid, status, created, changed

[users]                       ← User accounts
  uid, name, mail, pass, status

[comment]                     ← Comments on ideas
  cid, nid, uid, subject, comment_body, status

[taxonomy_term_data]          ← Technology/Category/Difficulty terms
  tid, vid, name

[field_data_field_status]     ← Idea approval status (Pending/Approved/Rejected)
  entity_id (= nid), field_status_value

[field_data_field_votes]      ← Vote count synced from votes table
  entity_id (= nid), field_votes_value

[field_data_field_trending_score]  ← Calculated trending score
  entity_id (= nid), field_trending_score_value

[field_data_field_event_status]    ← Event status (pending/approved/rejected)
  entity_id (= nid), field_event_status_value

[field_data_field_technology_tags]  ← Idea → Technology taxonomy links
  entity_id, field_technology_tags_tid

[field_data_field_related_event]    ← Idea → Event entity reference
  entity_id (= idea nid), field_related_event_target_id (= event nid)

[innovation_hub_votes]         ← Custom vote records (one row per vote)
  vid, nid, uid, created

[cache_innovation_hub]         ← Module's own cache storage
  cid, data, expire, created, serialized
```

### Entity Relationship Diagram (Text Version)

```
[users] ──────── creates ──────────> [node: hackathon_event]
                                              │
                                              │ field_related_event
                                              ▼
[users] ──────── submits ──────────> [node: project_idea]
                                              │
                 ┌─────────────────────────────┤
                 │                             │
                 ▼                             ▼
        [innovation_hub_votes]        [comment]
        (who voted for what)       (comments on idea)
                 │
                 └──── updates ──────> [field_data_field_votes]
                                       [field_data_field_trending_score]
```

---

## 8. The Voting System

### Step-by-Step Vote Flow

```
User clicks "Vote" button
        ↓
GET /idea/{nid}/vote
        ↓
innovation_hub_vote($node) runs
        ↓
[Check 1] Is it a project_idea? → No → 404 Not Found
        ↓
[Check 2] Is the idea Approved? → No → "Not approved yet" message
        ↓
[Check 3] Is the user the idea's author? → Yes → "Cannot self-vote"
        ↓
[Check 4] Has user already voted? → Yes → "Already voted" message
        ↓
[Check 5] Has user voted 10+ times this hour? → Yes → "Rate limit exceeded"
        ↓
[All checks passed] Begin database transaction
        ↓
INSERT into innovation_hub_votes (uid, nid, created=now)
        ↓
COUNT total votes for this idea
        ↓
UPDATE field_data_field_votes SET field_votes_value = total
        ↓
UPDATE field_revision_field_votes (revision history)
        ↓
Recalculate trending score → UPDATE field_data_field_trending_score
        ↓
Clear cache
        ↓
Show "Your vote has been recorded." message
        ↓
Redirect to node/{nid}
```

---

## 9. The Trending Algorithm

The trending score determines which ideas appear first on the homepage and in the Trending Ideas block.

### Formula

```
Score = (votes × 2) + comments + recency_bonus
```

### Explanation of Each Component

| Component | Value | Why |
|---|---|---|
| `votes × 2` | Each vote = 2 points | Votes are the primary signal of quality |
| `comments` | Each comment = 1 point | Discussion shows engagement |
| `recency_bonus` | 0–3 points | Fresh ideas get a boost |

### Recency Bonus Details

```
Age of idea:         Bonus:
0–3 days old    →    +3 points  (brand new – maximum boost)
4–7 days old    →    +1 point   (recent – small boost)
8+ days old     →    +0 points  (established – no boost)
```

### Examples

**Example 1:** A 2-day-old idea with 5 votes and 3 comments:
```
Score = (5 × 2) + 3 + 3 = 16
```

**Example 2:** A 10-day-old idea with 20 votes and 8 comments:
```
Score = (20 × 2) + 8 + 0 = 48
```

Even with more votes, the older idea scores higher because of its votes – but a newer idea can compete by getting fresh votes and comments.

### When Is the Score Updated?

- When a new idea is created (`hook_node_insert`)
- When an idea is edited (`hook_node_update`)
- When a comment is added (`hook_comment_insert`)
- When a vote is cast (`innovation_hub_vote`)

---

## 10. The Analytics Dashboard

Available at `/analytics`, the dashboard provides four visualizations:

### Chart 1 – Ideas by Technology (Bar Chart)

**Data:** SQL query groups approved ideas by their technology taxonomy tag.

```sql
SELECT t.name, COUNT(n.nid) AS total
FROM {taxonomy_term_data} t
LEFT JOIN {field_data_field_technology_tags} ft ON t.tid = ft.field_technology_tags_tid
LEFT JOIN {node} n ON n.nid = ft.entity_id AND n.type = 'project_idea'
LEFT JOIN {field_data_field_status} fs ON n.nid = fs.entity_id
WHERE fs.field_status_value = 'Approved'
GROUP BY t.tid HAVING total > 0
```

**Output:** Bar chart showing how many approved ideas use AI, Blockchain, IoT, etc.

### Chart 2 – Ideas by Event (Pie Chart)

**Data:** Groups ideas by which hackathon event they belong to.

**Output:** Pie chart showing which event attracted the most ideas.

### Chart 3 – Monthly Submissions (Line Chart)

**Data:** Groups approved ideas by year-month using `DATE_FORMAT(FROM_UNIXTIME(created), '%Y-%m')`.

**Output:** Line chart showing idea submission trends over time.

### Chart 4 – Top Contributors (HTML List)

**Data:** Lists the top 5 users with the most approved ideas.

**Output:** Numbered `<ol>` list.

### How Chart.js Is Used

Chart.js is loaded from a CDN:
```php
drupal_add_js('https://cdn.jsdelivr.net/npm/chart.js', 'external');
```

PHP passes the query results to JavaScript via `json_encode()`:
```php
labels: <?php echo json_encode($tech_labels); ?>,
data:   <?php echo json_encode($tech_values); ?>
```

The chart is initialized inside an **IIFE (Immediately Invoked Function Expression)**:
```javascript
(function() {
  "use strict";
  // Chart initialization code
})();
```
This keeps variables scoped and avoids polluting the global JavaScript namespace.

---

## 11. Admin Workflow: Event Moderation

### Pending Events Page (`/admin/pending-events`)

Admins can see all events with `field_event_status = 'pending'` in a table with columns: Event Title, Organizer, Submitted Date, and Actions (Approve/Reject buttons).

### Approval Flow

```
Admin clicks "Approve"
        ↓
GET /admin/events/approve/{nid}
        ↓
innovation_hub_event_approve($nid)
        ↓
Check: user has 'administer nodes' permission
        ↓
Check: node is a hackathon_event
        ↓
Begin database transaction
        ↓
UPDATE field_data_field_event_status SET value = 'approved'
UPDATE field_revision_field_event_status SET value = 'approved'
        ↓
Log to watchdog
        ↓
Clear Views cache
        ↓
Redirect to /admin/pending-events with success message
```

Rejection follows the same pattern but sets value to `'rejected'`.

---

## 12. Security Features

### 12.1 XSS (Cross-Site Scripting) Prevention

**What is XSS?** When a user submits `<script>alert('hacked')</script>` as a title, and the site displays it unescaped, the browser executes it.

**How we prevent it:** Every piece of user-generated text is passed through `check_plain()` before display:

```php
// Instead of this (DANGEROUS):
echo $row->title;

// We always do this (SAFE):
echo check_plain($row->title);
```

`check_plain()` converts `<` to `&lt;`, `>` to `&gt;`, `"` to `&quot;`, etc.

### 12.2 SQL Injection Prevention

**What is SQL injection?** When user input contains SQL code that modifies the query, like: `'; DROP TABLE users; --`

**How we prevent it:** We NEVER concatenate user input into SQL strings. We always use **parameterized queries** with named placeholders:

```php
// DANGEROUS (never do this):
db_query("SELECT * FROM node WHERE nid = " . $_GET['nid']);

// SAFE (always do this):
db_query("SELECT * FROM {node} WHERE nid = :nid", array(':nid' => $nid));
```

Drupal automatically escapes the values bound to `:nid`.

Test 13 in the test suite (`testSQLInjectionPrevention`) verifies that passing `'; DROP TABLE innovation_hub_votes; --` as a parameter does NOT drop the table.

### 12.3 Access Control

- The vote route uses `'access callback' => 'user_is_logged_in'` – anonymous users are blocked
- Admin routes check `user_access('administer nodes')` and call `drupal_access_denied()` if not authorized
- The vote handler checks `$node->uid == $user->uid` to prevent self-voting

### 12.4 Rate Limiting

Max 10 votes per hour per user. Prevents automated/spam voting.

### 12.5 Transaction Safety

All multi-step database operations (voting, approving, rejecting) are wrapped in `db_transaction()`. If any step fails, all changes are rolled back:

```php
$transaction = db_transaction();
try {
  // Multiple db operations...
} catch (Exception $e) {
  $transaction->rollback();  // Undo everything
}
```

---

## 13. Caching System

### Why Caching?

Database queries are expensive (slow). If 1000 users visit the homepage at the same time, we don't want to run the trending ideas query 1000 times. Caching stores the result once and serves it to everyone until the data changes.

### Cache Bins in Use

| Cache Bin | Used For |
|---|---|
| `cache_innovation_hub` | Rate-limit vote counts per user |
| `cache_block` | The rendered Trending Ideas block HTML |
| Drupal's built-in Views cache | Rendered Views output |

### Custom Cache Table

The `cache_innovation_hub` table follows Drupal's standard cache schema (cid, data, expire, created, serialized). It is registered in `hook_flush_caches()` so that when an admin clicks "Clear all caches", this bin is also cleared.

### When Cache Is Invalidated

Cache is cleared after:
- A vote is cast
- An event is approved or rejected

```php
function innovation_hub_clear_trending_cache() {
  views_invalidate_cache();  // Clear Views cache
  cache_clear_all('innovation_hub_votes:*', 'cache_block', TRUE);
  if (db_table_exists('cache_innovation_hub')) {
    cache_clear_all(NULL, 'cache_innovation_hub');
  }
}
```

### Safe Cache Checks

Before using `cache_get()`, the code checks if the cache table exists: `if (db_table_exists('cache_innovation_hub'))`. This prevents fatal errors during installation before the table is created.

---

## 14. Contributed Modules Explained

These are modules downloaded from Drupal.org and placed in `sites/all/modules/`.

| Module | What It Does in This Project |
|---|---|
| **Views** | Powers the `/events` and `/ideas` pages – displays lists of content from the database without writing custom SQL |
| **CTools (Chaos Tools)** | Required by Views and Panels; provides plugin and modal dialog systems |
| **Entity** | Extends Drupal's entity system; required for Entity Reference |
| **Entity Reference** | Creates the relationship between Project Ideas and Hackathon Events |
| **Field Group** | Groups fields together visually on node edit forms |
| **Field Permissions** | Controls which user roles can view/edit specific fields |
| **Webform** | Creates contact/feedback forms (drag-and-drop form builder) |
| **CAPTCHA** | Adds bot-prevention challenges to forms |
| **Login Security** | Adds brute-force protection to the login form |
| **Date** | Adds a date field type (used for event dates) |
| **Token** | Provides text replacement tokens like `[user:name]`, used by Pathauto |
| **Pathauto** | Automatically creates clean URLs from content titles |
| **Admin Menu** | Replaces the standard Drupal admin toolbar with a faster, better one |
| **SMTP** | Sends emails via an SMTP server instead of PHP mail() |
| **jQuery Update** | Updates Drupal's bundled jQuery version to a newer release |
| **Devel** | Developer tools (query debugger, variable dumper) – for development only |
| **DS (Display Suite)** | Controls the layout of how nodes are displayed |
| **Panels** | Advanced page layout manager |

---

## 15. The Bootstrap Theme

Located in `sites/all/themes/bootstrap/`, this is a Drupal theme that integrates **Bootstrap 3** into Drupal's theming system.

**What Bootstrap provides:**
- A **12-column responsive grid** (`col-md-4`, `col-md-3`, etc.) – used throughout the homepage
- **Button styles** (`btn`, `btn-primary`, `btn-success`, `btn-danger`)
- **Cards** – The idea cards and event cards use `card`, `card-body`, `card-footer`
- **Table styles** (`table table-striped`) – Used in the admin pending events page
- **Jumbotron** – The hero banner at the top of the homepage
- **Badges** – The difficulty label (Beginner/Intermediate/Advanced) on idea cards
- **Responsive design** – The site looks good on both mobile and desktop

---

## 16. Testing: SimpleTest / DrupalWebTestCase

### What Is SimpleTest?

Drupal 7 uses **SimpleTest**, a built-in PHP testing framework. Tests run in an isolated test environment that creates a fresh, temporary database, runs through scenarios, and then destroys the test database. This means tests never affect your real data.

### The Test Class

Located in `innovation_hub.test`:

```php
class InnovationHubTest extends DrupalWebTestCase {
  // ...
}
```

`DrupalWebTestCase` is the parent class that provides:
- `drupalCreateUser()` – Creates a test user
- `drupalLogin()` – Logs in as a user
- `drupalGet($path)` – Navigates to a URL
- `drupalPost($path, $edit, $button)` – Submits a form
- `assertText($text)` – Checks the page contains certain text
- `assertResponse($code)` – Checks the HTTP response code (200 = OK, 403 = Forbidden, 404 = Not Found)
- `assertTrue($condition, $message)` – Fails if condition is false
- `assertEqual($a, $b, $message)` – Fails if `$a !== $b`

### Test Setup (`setUp()`)

The `setUp()` method runs before every test. It:
1. Enables the required modules (`innovation_hub`, `node`, `comment`, `field`, `taxonomy`, `field_ui`)
2. Creates the `field_status` field programmatically for the test environment
3. Creates three test users: `regularUser`, `secondRegularUser`, `adminUser`
4. Logs in as the admin user

### The 30 Tests (Organized by Section)

#### Section 1 – Module Installation (Tests 1–3)
| Test | What It Verifies |
|---|---|
| `testModuleInstallation` | `module_exists('innovation_hub')` is true |
| `testDatabaseTables` | `innovation_hub_votes` and `cache_innovation_hub` tables exist |
| `testDatabaseIntegrity` | Core Drupal tables (node, users) exist |

#### Section 2 – Homepage & Basic CRUD (Tests 4–7)
| Test | What It Verifies |
|---|---|
| `testHomepageLoad` | `/` returns HTTP 200 and contains "Innovation Hub" |
| `testIdeaCreation` | Submitting the article form creates a node |
| `testIdeaEdit` | Editing a node updates its title in the database |
| `testIdeaDeletion` | `node_delete()` removes the node |

#### Section 3 – Workflow (Tests 8–10)
| Test | What It Verifies |
|---|---|
| `testIdeaApprovalWorkflow` | Setting `field_status = 'Approved'` persists |
| `testIdeaRejectionWorkflow` | Setting `field_status = 'Rejected'` persists |
| `testIdeaStatusWorkflow` | Setting `field_status = 'Pending'` persists |

#### Section 4 – Security (Tests 11–15)
| Test | What It Verifies |
|---|---|
| `testXSSPreventionTitle` | `<script>` tags are not rendered raw |
| `testXSSPreventionBody` | `onerror` attributes are stripped from body content |
| `testSQLInjectionPrevention` | Passing malicious SQL as a query parameter does not drop the votes table |
| `testAnonymousCannotCreateContent` | Anonymous users get HTTP 403 when accessing `node/add/article` |
| `testPublishedContentAccessible` | Published nodes are accessible to anonymous users |

#### Section 5 – Form Validation (Tests 16–18)
| Test | What It Verifies |
|---|---|
| `testFormValidationEmptyTitle` | Empty title does not cause a fatal error |
| `testFormValidationSpecialCharacters` | Special characters in titles are accepted |
| `testFormSubmissionWorks` | Valid form submission creates the node |

#### Section 6 – Voting System (Tests 19–21)
| Test | What It Verifies |
|---|---|
| `testVotingSystem` | New idea has 0 votes in `innovation_hub_votes` |
| `testVotePageAccess` | Navigating to the vote URL completes without fatal error |
| `testTrendingScoreCalculation` | Score is a numeric value (or gracefully skipped if field not in test DB) |

#### Section 7 – Analytics (Tests 22–23)
| Test | What It Verifies |
|---|---|
| `testAnalyticsDashboard` | `/analytics` returns HTTP 200 with dashboard title |
| `testIdeaCountQuery` | SQL COUNT query on nodes returns correct results |

#### Section 8 – Multi-User (Tests 24–27)
| Test | What It Verifies |
|---|---|
| `testMultipleUsersCreateIdeas` | Two users can each create their own ideas |
| `testMultipleUserComments` | Two users can comment on the same idea |
| `testNodeRevisionsTracked` | Saving a node with `revision = TRUE` creates a revision |
| `testNodeOwnership` | Node's `uid` matches the creator's `uid` |

#### Section 9 – Content Types (Tests 28–30)
| Test | What It Verifies |
|---|---|
| `testContentTypeAccessibility` | Authenticated users can access `node/add/article` |
| `testUserCanEditOwnContent` | User can edit a node they own |
| `testContentTypeExists` | `article` content type exists in the system |

### How to Run Tests

In Drupal 7, tests are run through:
- **Drupal Admin UI:** `admin/config/development/testing`
- **Command line:** `php scripts/run-tests.sh --class InnovationHubTest`

---

## 17. How Everything Connects – System Flow

```
USER VISITS /home
      ↓
innovation_hub_homepage() runs
      ↓
Queries database for:
  - Platform stats (ideas, events, users, votes)
  - Top 5 trending approved ideas
  - Top 3 approved upcoming events
      ↓
Renders HTML with Bootstrap grid layout
      ↓
theme('innovation_hub_homepage', $data) → renders tpl.php template
      ↓
Chart.js renders platform statistics bar chart
      ↓
Page is returned to browser


USER SUBMITS AN IDEA
      ↓
Fills out node/add/project-idea form
      ↓
innovation_hub_form_alter() added validation runs:
  - Title must be 5+ characters
  - Body must be 20+ characters
      ↓
Node is saved → innovation_hub_node_insert() fires:
  - Sets field_status = 'Pending'
  - Initializes trending score = 0
      ↓
Admin sees idea in pending list
      ↓
Admin approves idea (via Drupal admin or custom view)


USER VOTES ON AN IDEA
      ↓
Clicks Vote button → GET /idea/{nid}/vote
      ↓
5 security checks run
      ↓
Vote recorded in innovation_hub_votes table
      ↓
field_votes_value updated
      ↓
Trending score recalculated: (votes×2) + comments + recency_bonus
      ↓
Cache cleared
      ↓
Idea moves up in trending rankings
```

---

## 18. Q&A Preparation – Senior Developer Questions

---

### Q: What is a Drupal hook and why is it important?

**A:** A hook is Drupal's event system. Drupal calls a function with a specific name at predefined points in its execution (e.g., when a node is saved, when a page renders). Any module can implement that function to inject custom behavior. For example, `hook_node_insert()` fires whenever any content is saved, so our module can set the default status to "Pending" for new ideas without modifying Drupal's core code. This keeps our custom logic separate from Drupal's core, which is important for upgrades and maintainability.

---

### Q: Why do you use database transactions?

**A:** A transaction ensures that multiple related database operations either all succeed or all fail together. For example, when recording a vote, we: (1) insert into `innovation_hub_votes`, (2) update `field_data_field_votes`, and (3) update `field_revision_field_votes`. If step 2 fails, step 1 would have already happened, leaving the database in an inconsistent state. By wrapping everything in `db_transaction()`, Drupal will roll back all changes if any error occurs, so the data is always consistent.

---

### Q: How do you prevent SQL injection in this project?

**A:** We never concatenate user input directly into SQL strings. We use Drupal's Database API with named placeholders like `:nid`. Drupal's database layer automatically escapes all values bound to placeholders, making SQL injection impossible. The test `testSQLInjectionPrevention` verifies this by passing `'; DROP TABLE innovation_hub_votes; --` as a query parameter and confirming the table still exists afterward.

---

### Q: What is `check_plain()` and why must you use it?

**A:** `check_plain()` is Drupal's HTML escaping function. It converts special HTML characters (`<`, `>`, `"`, `&`) to their HTML entity equivalents (`&lt;`, `&gt;`, etc.). This prevents XSS (Cross-Site Scripting) attacks where a malicious user could inject `<script>alert('hacked')</script>` as their idea title. Without escaping, the browser would execute that script when the page loads. Every user-generated text value must be passed through `check_plain()` before being output to a page.

---

### Q: What is caching and why does this project use it?

**A:** Caching stores computed results (like database query results or rendered HTML) temporarily so they don't have to be recomputed on every request. This project uses two types: (1) **Block cache** – the Trending Ideas block HTML is cached globally (`DRUPAL_CACHE_GLOBAL`), so the trending query runs once per cache period instead of on every page load. (2) **Custom cache bin** (`cache_innovation_hub`) – stores vote rate-limit counts per user so we don't query the database on every vote attempt. The cache is invalidated (cleared) whenever data changes (a new vote, an approval), so users always see fresh data.

---

### Q: Explain the trending algorithm.

**A:** The trending score is calculated as: `(votes × 2) + comments + recency_bonus`. Votes are weighted double because they are a direct signal of quality. Comments add 1 point each because discussion shows engagement. The recency bonus (+3 for ideas 0–3 days old, +1 for 4–7 days, 0 after that) ensures fresh ideas can compete with established ones that have accumulated more votes over time. The score is stored in `field_data_field_trending_score` and updated every time a vote is cast, a comment is added, or an idea is edited.

---

### Q: How does the voting system prevent abuse?

**A:** The system has five layers of protection: (1) Login required – anonymous users cannot vote. (2) Approval required – you can only vote on approved ideas, not pending/rejected ones. (3) Self-vote prevention – the author of an idea cannot vote for their own idea. (4) Duplicate vote prevention – enforced both in code (checks the `innovation_hub_votes` table) and at the database level (unique key on `uid + nid`). (5) Rate limiting – maximum 10 votes per hour per user, checked against a timestamp in the votes table.

---

### Q: What is the difference between `hook_schema()` and `hook_update_N()`?

**A:** `hook_schema()` defines the database structure that Drupal creates when the module is first installed. `hook_update_N()` functions (numbered like `_update_8001`, `_update_8002`) run during module updates for sites that already have the module installed. For example, if we add a new index to the votes table after deployment, we can't change `hook_schema()` (because it already ran on existing sites), so we write an `hook_update_N()` function that adds the index to existing databases. Drupal tracks which update functions have run in the `system` table.

**Note on the `8xxx` numbering:** You may notice that in a Drupal 7 project, the convention is normally to use `7xxx` numbering for these functions. This project uses `8xxx` (e.g., `innovation_hub_update_8001`). Both work correctly — Drupal 7 simply runs all unexecuted `hook_update_N` functions in ascending order regardless of the starting number. The `8xxx` series is an unusual but valid stylistic choice and does not indicate this is a Drupal 8 module.

---

### Q: Why is there a `field_revision_` table and why do you update it?

**A:** Drupal maintains two versions of each field table: `field_data_[field_name]` (the current value) and `field_revision_[field_name]` (the history of all revisions). When a node is revised, the revision table records what the field value was at each point in time. If we only update `field_data_`, the current value is correct, but the revision history becomes inconsistent. We update both tables to keep the vote count and status accurate in both the current state and the revision history.

---

### Q: What is DrupalWebTestCase and how does the test setup work?

**A:** `DrupalWebTestCase` is Drupal 7's testing class that simulates a full web browser interacting with a temporary Drupal installation. Each test run creates a completely separate test database with a fresh Drupal install. In `setUp()`, we enable the modules we need, create test content types and fields programmatically, and create test user accounts. Each test function then uses methods like `drupalGet()`, `drupalPost()`, and `assertText()` to simulate user actions and verify outcomes. When all tests finish, the test database is destroyed, leaving the real database untouched.

---

### Q: What is the purpose of `hook_form_alter()`?

**A:** `hook_form_alter()` lets you modify any form in Drupal without touching the code that originally creates the form. It receives the form array by reference, along with the form state and form ID. In this project, we use it to modify the project idea submission form: adding a placeholder to the title field, adding descriptive text, and registering a custom validation function. This follows Drupal's principle of non-destructive customization – we extend behavior without overwriting the original code.

---

### Q: Why does the module check `db_table_exists()` before querying custom tables?

**A:** During module installation, there is a brief moment when the module's PHP code is loaded but the database tables (created by `hook_schema()`) may not exist yet. This can also happen in test environments where not all features are set up. Checking `db_table_exists()` before querying prevents fatal errors during installation, updates, or test runs. It is a defensive coding practice that makes the module more resilient.

---

### Q: What is Entity Reference and why is it used here?

**A:** Entity Reference is a field type that creates a relationship between two pieces of content (entities). In InnovationHub, a Project Idea has an `entity_reference` field (`field_related_event`) that points to a Hackathon Event node. This is like a foreign key in a relational database, but managed through Drupal's field system. It allows the Analytics Dashboard to query "how many ideas belong to each event" and lets ideas be grouped under their parent event. The relationship is stored in `field_data_field_related_event` as `(entity_id=idea_nid, field_related_event_target_id=event_nid)`.

---

*End of Documentation*

---

**Document prepared for:** InnovationHub Project Presentation  
**Drupal Version:** 7.95  
**Module Version:** 7.x-1.0  
**Author:** Shreeti Bajracharya
