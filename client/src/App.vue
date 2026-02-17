<script setup>
import { ref, onMounted } from 'vue';
import { load } from './lib/golf.js';
import GolfDemo from './demos/GolfDemo.vue';

const ready = ref(false);
const error = ref(null);

onMounted(async () => {
  try {
    await load();
    ready.value = true;
  } catch (e) {
    error.value = e.message;
  }
});
</script>

<template>
  <div class="app">
    <header>
      <h1>@thestonepixel/physics</h1>
      <p class="subtitle">Live Demo — C physics engine running in your browser via WebAssembly</p>
    </header>

    <div v-if="error" class="error">Failed to load WASM: {{ error }}</div>
    <div v-else-if="!ready" class="loading">Loading physics engine...</div>

    <template v-else>
      <nav class="demo-nav">
        <span class="nav-item active">Golf</span>
        <span class="nav-item disabled">More demos coming soon</span>
      </nav>
      <GolfDemo />
    </template>
  </div>
</template>

<style>
* { margin: 0; padding: 0; box-sizing: border-box; }

body {
  background: #0d1117;
  color: #e0e0e0;
  font-family: system-ui, -apple-system, sans-serif;
}

.app {
  max-width: 960px;
  margin: 0 auto;
  padding: 24px 20px;
}

header {
  margin-bottom: 24px;
}

header h1 {
  font-size: 1.3rem;
  font-weight: 600;
  color: #4af;
  font-family: monospace;
}

.subtitle {
  font-size: 0.85rem;
  color: #5a7a9a;
  margin-top: 4px;
}

.loading, .error {
  padding: 40px;
  text-align: center;
  font-size: 0.9rem;
}

.error { color: #f44; }
.loading { color: #5a7a9a; }

.demo-nav {
  display: flex;
  gap: 16px;
  margin-bottom: 24px;
  border-bottom: 1px solid #1a2a44;
  padding-bottom: 12px;
}

.nav-item {
  font-size: 0.85rem;
  padding: 4px 12px;
  border-radius: 4px;
  cursor: default;
}

.nav-item.active {
  background: #1a2a44;
  color: #4af;
}

.nav-item.disabled {
  color: #3a4a5a;
  font-style: italic;
}
</style>
