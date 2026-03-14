<?php if (!empty($ideas)): ?>

<?php
  usort($ideas, function($a, $b) {
    $scoreA = isset($a->score) ? (int)$a->score : 0;
    $scoreB = isset($b->score) ? (int)$b->score : 0;
    return $scoreB - $scoreA;
  });

  $featured_idea = $ideas[0];
?>

<section class="trending-ideas-section">

<div class="container text-center">

<h2 class="section-title">💡 <?php print t('Idea of the Day'); ?></h2>

<div class="row">

<div class="col-md-6" style="margin:auto;">

<div class="idea-card">

<span class="rank-badge">Trending</span>

<div class="card-body">

<h3 class="idea-title">
<a href="<?php print url('node/' . $featured_idea->nid); ?>">
<?php print check_plain($featured_idea->title); ?>
</a>
</h3>

<?php if (!empty($featured_idea->difficulty)): ?>
<span class="badge badge-info">
<?php print check_plain($featured_idea->difficulty); ?>
</span>
<?php endif; ?>

<div class="idea-stats">

<span class="votes">
<?php print (int)$featured_idea->votes; ?> Votes
</span>

<span class="score">
<?php print (int)$featured_idea->score; ?> Score
</span>

</div>

</div>

<div class="card-footer">

<a href="<?php print url('node/' . $featured_idea->nid); ?>" class="btn btn-primary btn-sm">
<?php print t('View Idea'); ?>
</a>

<a href="<?php print url('idea/' . $featured_idea->nid . '/vote'); ?>" class="btn btn-outline-success btn-sm">
<?php print t('Vote'); ?>
</a>

</div>

</div>

</div>

</div>

</div>

</section>

<?php else: ?>

<div class="container text-center mt-4">
<p><?php print t('No ideas submitted yet. Be the first to add one!'); ?></p>
</div>

<?php endif; ?>