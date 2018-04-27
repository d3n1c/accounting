<?php $cleanurl = variable_get('clean_url', 0); ?>

<header class="app-header navbar">
  <button class="navbar-toggler mobile-sidebar-toggler d-lg-none mr-auto" type="button">
    <span class="navbar-toggler-icon"></span>
  </button>
  <a class="navbar-brand" href="<?php echo url('<front>'); ?>"></a>
  <button class="navbar-toggler sidebar-toggler d-md-down-none" type="button">
    <span class="navbar-toggler-icon"></span>
  </button>

  <?php if (!empty($page['header'])) { ?>
    <ul class="nav navbar-nav d-md-down-none">
      <?php foreach ($page['header'] as $values) { ?>
        <?php if (empty($values['#block'])) { continue; } ?>
        <li class="nav-item px-3">
          <?php print render($values); ?>
        </li>
      <?php } ?>
    </ul>
  <?php } ?>

  <ul class="nav navbar-nav ml-auto">
    <?php if (!empty($page['navigation'])) { ?>
      <?php foreach ($page['navigation'] as $values) { ?>
        <?php if (empty($values['#block'])) { continue; } ?>
        <li class="nav-item dropdown">
          <?php print render($values); ?>
        </li>
      <?php } ?>
    <?php } ?>
    <li class="nav-item dropdown">
      <a class="nav-link dropdown-toggle nav-link" data-toggle="dropdown" href="#" role="button" aria-haspopup="true" aria-expanded="false">
        <?php if (!empty($auth_user)) { ?>
          <?php echo $auth_user['user_picture']; ?>
        <?php } else { ?>
          <img src="<?php echo base_path() . path_to_theme() . '/img/logo-symbol.png'; ?>" class="img-avatar" alt="" />
        <?php } ?>
      </a>
      <div class="dropdown-menu dropdown-menu-right">
        <?php if (!empty($auth_user)) { ?>
          <?php if (!empty($auth_user['menus'])) { ?>
            <div class="dropdown-header text-center">
              <strong><?php echo t('@user\'s Account', ['@user' => $GLOBALS['user']->name]); ?></strong>
            </div>
            <?php foreach ($auth_user['menus'] as $keys => $values) { ?>
              <a class="dropdown-item" href="<?php echo (preg_match('/^http/i', $values['href']) ? NULL : (!empty($cleanurl) && !empty($values['#href']) && $values['#href'] != '<front>' && $values['#href'] != '/' ? base_path() : '?q=')) . $values['href']; ?>"><i class="fa <?php echo preg_match('/logout/i', $values['href']) ? 'fa-sign-out' : 'fa-circle-thin'; ?>"></i> <?php echo t($values['title']); ?></a>
            <?php } ?>
          <?php } ?>
        <?php } else { ?>
          <a class="dropdown-item" href="<?php echo (!empty($cleanurl) ? base_path() : '?q='); ?>user/login"><i class="fa fa-sign-in"></i> <?php echo t('Login'); ?></a>
        <?php } ?>
      </div>
    </li>
  </ul>
  <button class="navbar-toggler aside-menu-toggler" type="button">
    <span class="navbar-toggler-icon"></span>
  </button>

</header>

<?php unset($cleanurl); ?>