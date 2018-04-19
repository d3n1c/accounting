<aside class="aside-menu">
  <ul class="nav nav-tabs" role="tablist">
    <?php if (!empty($page['sidebar_second'])) { ?>
      <li class="nav-item">
        <a class="nav-link active" data-toggle="tab" href="#secondary" role="tab"><i class="icon-pin"></i></a>
      </li>
    <?php } ?>
    <?php if (module_exists('dnccoreui_assist') && user_is_logged_in()) { ?>
      <li class="nav-item">
        <a class="nav-link<?php echo empty($page['sidebar_second']) ? ' active' : NULL; ?>" data-toggle="tab" href="#example" role="tab"><i class="icon-book-open"></i></a>
      </li>
      <!-- <li class="nav-item">
        <a class="nav-link" data-toggle="tab" href="#settings" role="tab"><i class="icon-settings"></i></a>
      </li> -->
    <?php } ?>
  </ul>

  <!-- Tab panes -->
  <div class="tab-content">
    <?php if (!empty($page['sidebar_second'])) { ?>
      <div class="tab-pane active" id="secondary" role="tabpanel">
        <?php foreach ($page['sidebar_second'] as $keys => $values) { ?>
          <?php if (empty($values['#block'])) { continue; } ?>
          <?php if (!empty($values['#block']->subject)) { ?>
            <div class="callout m-0 py-2 text-muted text-center bg-light text-uppercase">
              <small><b><?php print($values['#block']->subject); ?></b></small>
            </div>
          <?php } ?>
          <hr class="transparent mx-3 my-0">
          <div class="callout callout-info m-0 py-3">
            <?php if (!empty($values['#theme'])) { ?>
              <?php print render($values); ?>
            <?php } elseif (!empty($values['#markup'])) { ?>
              <?php print($values['#markup']);?>
            <?php } ?>
          </div>
          <hr class="mx-3 my-0">
        <?php } ?>
      </div>
    <?php } ?>

    <?php if (module_exists('dnccoreui_assist') && user_is_logged_in()) { ?>
      <div class="tab-pane<?php echo empty($page['sidebar_second']) ? ' active' : NULL; ?>" id="example" role="tabpanel">
        <div class="callout m-0 py-2 text-muted text-center bg-light text-uppercase">
          <small><b><?php echo t('Theme'); ?></b></small>
        </div>
        <hr class="transparent mx-3 my-0">
        <div class="callout callout-danger m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/dashboard"><i class="fa fa-dashboard"></i> <?php echo t('Dashboard'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-danger m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/typography"><i class="fa fa-text-width"></i> <?php echo t('Typography'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-danger m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/widgets"><i class="icon-calculator"></i> <?php echo t('Widgets'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-danger m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/charts"><i class="icon-pie-chart"></i> <?php echo t('Charts'); ?></a>
        </div>

        <div class="callout m-0 py-2 text-muted text-center bg-light text-uppercase">
          <small><b><?php echo t('Icons'); ?></b></small>
        </div>
        <hr class="transparent mx-3 my-0">
        <div class="callout callout-warning m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/font-awesome"><i class="fa fa-font"></i> <?php echo t('Font Awesome'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-warning m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/simple-line-icons"><i class="icon-star"></i> <?php echo t('Simple Icons'); ?></a>
        </div>

        <div class="callout m-0 py-2 text-muted text-center bg-light text-uppercase">
          <small><b><?php echo t('Components'); ?></b></small>
        </div>
        <hr class="transparent mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/buttons"><i class="icon-social-youtube"></i> <?php echo t('Buttons'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/cards"><i class="fa fa-credit-card"></i> <?php echo t('Cards'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/forms"><i class="fa fa-window-maximize"></i> <?php echo t('Forms'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/modals"><i class="fa fa-sticky-note"></i> <?php echo t('Modals'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/social-buttons"><i class="fa fa-facebook-official"></i> <?php echo t('Social Buttons'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/switches"><i class="fa fa-toggle-on"></i> <?php echo t('Switches'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/tables"><i class="fa fa-table"></i> <?php echo t('Tables'); ?></a>
        </div>
        <hr class="mx-3 my-0">
        <div class="callout callout-success m-0 py-3">
          <a href="<?php echo base_path(); ?>dnccoreui/examples/tabs"><i class="fa fa-folder-o"></i> <?php echo t('Tabs'); ?></a>
        </div>
        <div class="callout callout-success m-0 py-3">
          &nbsp;
        </div>
      </div>

      <!--
      <div class="tab-pane p-3" id="settings" role="tabpanel">
        <h6>Settings</h6>

        <div class="aside-options">
          <div class="clearfix mt-4">
            <small><b>Option 1</b></small>
            <label class="switch switch-text switch-pill switch-success switch-sm float-right">
              <input type="checkbox" class="switch-input" checked>
              <span class="switch-label" data-on="On" data-off="Off"></span>
              <span class="switch-handle"></span>
            </label>
          </div>
          <div>
            <small class="text-muted">Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.</small>
          </div>
        </div>

        <div class="aside-options">
          <div class="clearfix mt-3">
            <small><b>Option 2</b></small>
            <label class="switch switch-text switch-pill switch-success switch-sm float-right">
              <input type="checkbox" class="switch-input">
              <span class="switch-label" data-on="On" data-off="Off"></span>
              <span class="switch-handle"></span>
            </label>
          </div>
          <div>
            <small class="text-muted">Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.</small>
          </div>
        </div>

        <div class="aside-options">
          <div class="clearfix mt-3">
            <small><b>Option 3</b></small>
            <label class="switch switch-text switch-pill switch-success switch-sm float-right">
              <input type="checkbox" class="switch-input">
              <span class="switch-label" data-on="On" data-off="Off"></span>
              <span class="switch-handle"></span>
            </label>
          </div>
        </div>

        <div class="aside-options">
          <div class="clearfix mt-3">
            <small><b>Option 4</b></small>
            <label class="switch switch-text switch-pill switch-success switch-sm float-right">
              <input type="checkbox" class="switch-input" checked>
              <span class="switch-label" data-on="On" data-off="Off"></span>
              <span class="switch-handle"></span>
            </label>
          </div>
        </div>

        <hr>
        <h6>System Utilization</h6>

        <div class="text-uppercase mb-1 mt-4">
          <small><b>CPU Usage</b></small>
        </div>
        <div class="progress progress-xs">
          <div class="progress-bar bg-info" role="progressbar" style="width: 25%" aria-valuenow="25" aria-valuemin="0" aria-valuemax="100"></div>
        </div>
        <small class="text-muted">348 Processes. 1/4 Cores.</small>

        <div class="text-uppercase mb-1 mt-2">
          <small><b>Memory Usage</b></small>
        </div>
        <div class="progress progress-xs">
          <div class="progress-bar bg-warning" role="progressbar" style="width: 70%" aria-valuenow="70" aria-valuemin="0" aria-valuemax="100"></div>
        </div>
        <small class="text-muted">11444GB/16384MB</small>

        <div class="text-uppercase mb-1 mt-2">
          <small><b>SSD 1 Usage</b></small>
        </div>
        <div class="progress progress-xs">
          <div class="progress-bar bg-danger" role="progressbar" style="width: 95%" aria-valuenow="95" aria-valuemin="0" aria-valuemax="100"></div>
        </div>
        <small class="text-muted">243GB/256GB</small>

        <div class="text-uppercase mb-1 mt-2">
          <small><b>SSD 2 Usage</b></small>
        </div>
        <div class="progress progress-xs">
          <div class="progress-bar bg-success" role="progressbar" style="width: 10%" aria-valuenow="10" aria-valuemin="0" aria-valuemax="100"></div>
        </div>
        <small class="text-muted">25GB/256GB</small>
      </div>
    -->

    <?php } ?>

  </div>
</aside>
