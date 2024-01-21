<script setup>
import { ref, onMounted } from 'vue';
const userName = ref(localStorage.userName);

</script>

<template>
  <div class="card">
    <h1>Welcome to JasonFan's BookStore</h1>
    <p>
    这是 JasonFan 的 Book Store System （SJTU ACM 班 2023 级程序设计课程大作业） Web 前端。
    用 <a href="https://cn.vuejs.org/">Vue.js</a> 和 <a href="https://primevue.org/">PrimeVue</a> 框架实现，使用了 <a href="https://github.com/primefaces/sakai-vue">SAKAI</a> 模板。
    </p>
    <br/>
      Login as <strong>{{ userName }}</strong>
    

  </div>
</template>
