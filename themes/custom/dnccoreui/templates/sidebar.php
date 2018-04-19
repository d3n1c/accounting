<div class="sidebar">
  <nav class="sidebar-nav">
    <ul class="nav">
      <li class="nav-item">
        <a class="nav-link" href="<?php echo url('<front>'); ?>"><i class="icon-home"></i> <?php echo t('Home'); ?> </a>
      </li>

      <?php if (!empty($sidebar_first_menus)) { ?>
        <?php foreach($sidebar_first_menus as $keys => $values) { ?>
          <li class="nav-title">
            <?php echo ucwords(t($keys)); ?>
          </li>
          <?php foreach ($values as $key => $value) { ?>
            <?php if (!empty($value['#children'])) { ?>
              <li class="nav-item nav-dropdown">
                <a class="nav-link nav-dropdown-toggle" href="#"><?php echo $value['#title']; ?></a>
                <ul class="nav-dropdown-items">
                  <?php foreach ($value['#children'] as $ke => $val) { ?>
                    <li class="nav-item">
                      <a class="nav-link small" style="text-align: right" href="<?php echo $val['#href']; ?>"><?php echo $val['#title']; ?></a>
                    </li>
                  <?php } ?>
                </ul>
              </li>
            <?php } else { ?>
              <li class="nav-item">
                <a class="nav-link" href="<?php echo $value['#href']; ?>"><?php echo $value['#title']; ?></a>
              </li>
            <?php } ?>
          <?php } ?>
        <?php } ?>
      <?php } ?>
    </ul>
  </nav>
  <button class="sidebar-minimizer brand-minimizer" type="button"></button>
</div>
