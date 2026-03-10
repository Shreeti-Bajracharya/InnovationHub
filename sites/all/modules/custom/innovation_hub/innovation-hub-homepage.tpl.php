<div class="innovation-homepage">

<h1>Innovation Hub</h1>
<p>Collaborate, build ideas, and participate in hackathons.</p>

<h2>Trending Ideas</h2>

<div class="row">

<?php foreach ($ideas as $idea): ?>

<div class="col-md-4">

<div class="idea-card">

<h3><?php print $idea->title; ?></h3>

<p>Votes: <?php print $idea->votes; ?></p>

</div>

</div>

<?php endforeach; ?>

</div>

</div>