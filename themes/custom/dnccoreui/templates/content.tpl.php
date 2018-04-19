<div class="animated fadeIn">
  <div class="card card-default">
    <?php if (!empty($title)): ?>
      <div class="card-header">
        <?php print render($title_prefix); ?>
        <strong><?php print ucwords(strtolower($title)); ?></strong>
        <?php print render($title_suffix); ?>
      </div>
    <?php endif; ?>
    <div class="card-body">
      <?php print render($page['content']); ?>
    </div>
  </div>
</div>
